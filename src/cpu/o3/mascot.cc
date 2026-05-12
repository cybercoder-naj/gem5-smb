/**
 * Copyright ???
 */

#include "cpu/o3/mascot.hh"

#include <cassert>
#include <cmath>

#include "cpu/o3/mem_dep_unit.hh"
#include "params/BaseO3CPU.hh"

namespace gem5
{

namespace o3
{

using Prediction = MASCOT::Prediction;
  
MASCOT::MASCOT()
  : depCheckShift(0),
    memDepUnit(nullptr),
    histories({0, 2, 4, 8, 16, 32, 64, 128}),
    tables(histories.size(), Table()),
    storeBuffer(MAX_DISTANCE) {}

void
MASCOT::init(const BaseO3CPUParams& params, MemDepUnit* mem_dep_unit) {
  depCheckShift = params.LSQDepCheckShift;
  memDepUnit = mem_dep_unit;
}

Prediction
MASCOT::predict(Addr load_pc, InstSeqNum load_seq_num, BranchHistory branch_history) {
  Prediction prediction { 
    .type = NDEP, 
    .distance = 0,
    .tableIdx = 0,
    .hash = 0
  };
  if (branch_history.empty())
    return prediction;

  // Branch history is added to the front of the deque
  // Earlier the entry, the more recent branch
  // Finds younger branches info
  // branch <- branch <- load <- branch <- branch <- branch <- store
  //                             ^ begin
  unsigned historyBegin = 0;
  while (historyBegin < branch_history.size() && branch_history[historyBegin].seqNum > load_seq_num) {
      historyBegin++;
  }
  //? bug?
  if (historyBegin > branch_history.size()) return prediction; //no +1 branch

  auto table_limit_idx = 0;
  while (table_limit_idx + 1 < histories.size() &&
         histories[table_limit_idx + 1] < branch_history.size()) { 
    ++table_limit_idx; 
  }

  //? Should I get lowest first?
  for (long i = table_limit_idx - 1; i >= 0; --i) {
    const uint64_t hash = generateBranchHash(histories[i], branch_history, historyBegin);
    const auto entry = tables[i].getEntry(load_pc, hash);
    if (entry == nullptr)
      continue;

    prediction.distance = entry->distance;
    prediction.tableIdx = i;
    prediction.hash = hash;

    // "A distance field of all 0s indicates that the entry is nondependent"
    if (entry->isNdep())
      return prediction;

    // "Whenever the distance field is not zero, a memory dependence prediction
    //  is made regardless of the value of the usefulness field, whereas
    //  speculative memory bypassing is only predicted if both the usefulness
    //  and bypassing counters are saturated."
    if (entry->isHighConfidence() && entry->canBypass() && entry->distance <= storeBuffer.size()) {
      auto store_idx = storeBuffer.size() - entry->distance;
      prediction.type = storeBuffer[store_idx].isStore ? SMB : MDP; 

      if (prediction.type == SMB) {
        prediction.storeSeqNum = storeBuffer[store_idx].seq_num;
        prediction.storePC = storeBuffer[store_idx].isStore;
      }
    }
    ++(*(memDepUnit->pathWrites[i])); //? Shouldn't this be pathReads??


    return prediction;
  }

  return prediction;
}

void 
MASCOT::commit(Addr load_pc,
                std::pair<Addr, unsigned> load_addr,
                std::pair<Addr, unsigned> store_addr,
                std::ptrdiff_t actual_sq_dist,
                BranchHistory branch_history,
                Prediction prediction) {
  Addr load_addr_start = load_addr.first >> depCheckShift;
  Addr load_addr_end = (load_addr.first + load_addr.second - 1) >> depCheckShift;

  Addr store_addr_start = store_addr.first >> depCheckShift;
  Addr store_addr_end = (store_addr.first + store_addr.second - 1) >> depCheckShift;

  // misprediction == true iff MDP/SMB was predicted but it didn't have to.
  bool misprediction = !(load_addr_start <= store_addr_end && store_addr_start <= load_addr_end);

  tables[prediction.tableIdx].commit(load_pc, prediction.hash, misprediction);
  ++(*(memDepUnit->pathReads[prediction.tableIdx]));
  ++(*(memDepUnit->pathWrites[prediction.tableIdx]));

  if (misprediction) {
    // Allocate non dependency in next table.
    allocateEntry(prediction.tableIdx + 1, load_pc, branch_history, actual_sq_dist, true);
    ++(memDepUnit->stats.falseDependencies);
  } else {
    ++(memDepUnit->stats.correctPredictions);
  }
}

void
MASCOT::violation(Addr load_pc,
                  InstSeqNum store_seq_num,
                  std::ptrdiff_t actual_sq_dist,
                  bool predicted,
                  Prediction prediction,
                  BranchHistory branch_history) {
  // History is newest-first. back() is the oldest branch — if it's still
  // newer than the store, no +1 branch exists and we cannot form a history
  if (branch_history.empty() || branch_history.back().seqNum > store_seq_num) 
    return;

  // Walk from the load end to the +1 branch (the first branch at or before
  // the store), then step past it so it's included in the count.
  auto br_it = branch_history.begin();
  do {
    br_it++; // includes +1 branch.
  } while (br_it != branch_history.end() && br_it->seqNum > store_seq_num);

  const unsigned actual_branches = static_cast<unsigned>(std::distance(branch_history.begin(), br_it));

  unsigned table_idx = 0;
  while (table_idx + 1 < histories.size() &&
        histories[table_idx + 1] <= actual_branches) {
    ++table_idx;
  }
  const auto historySize = histories[table_idx];

  if (predicted) {
    tables[prediction.tableIdx].commit(load_pc, prediction.hash, true);
    ++(memDepUnit->stats.falseDependencies);
    ++(*(memDepUnit->pathReads[prediction.tableIdx]));
    ++(*(memDepUnit->pathWrites[prediction.tableIdx]));
  }

  allocateEntry(prediction.tableIdx + 1, load_pc, branch_history, actual_sq_dist, false);
}

void
MASCOT::allocateEntry(const unsigned startTableIdx,
                      Addr load_pc,
                      BranchHistory branch_history,
                      std::ptrdiff_t sq_dist,
                      bool non_dep) {
  if (startTableIdx >= tables.size()) return;

  auto idx = startTableIdx;
  uint64_t hash = generateBranchHash(histories[idx], branch_history, 0);

  if (tables[idx].tryAllocate(load_pc, hash, sq_dist, non_dep))
    return;

  tables[idx].decrConfidence(load_pc, hash);

  while (++idx < tables.size()) {
    hash = generateBranchHash(histories[idx], branch_history, 0);
    if (tables[idx].tryAllocate(load_pc, hash, sq_dist, non_dep))
      return;
  }
}

void
MASCOT::clear() {
  for (auto& table : tables) {
    table.clear();
  }
}

void
MASCOT::pushStore(InstSeqNum store_seq_num, Addr pc, bool is_store) {
  storeBuffer.advance_tail();

  StoreBufferEntry entry {
    .seq_num = store_seq_num,
    .pc = pc,
    .isStore = is_store
  };
  storeBuffer.back() = entry;
}

void
MASCOT::popStores(InstSeqNum squashed_seq_num) {
  while (storeBuffer.size() != 0 &&
         storeBuffer.back().seq_num > squashed_seq_num)
    storeBuffer.pop_back();
}

void 
MASCOT::removeStores(InstSeqNum seq_num) {
  while (storeBuffer.size() != 0 &&
         storeBuffer.front().seq_num <= seq_num)
    storeBuffer.pop_front();
}

void
MASCOT::Table::commit(Addr load_pc, uint64_t hash, bool misprediction) {
  auto entry = getEntry(load_pc, hash);
  if (entry == nullptr)
    return;

  if (misprediction) {
    entry->decrConfidence();
    entry->resetCanBypass();
  } else {
    entry->incrConfidence();
    entry->incrCanBypass();
  }
}

bool
MASCOT::Table::tryAllocate(Addr load_pc, uint64_t hash, std::ptrdiff_t sq_dist, bool non_dep) {
  auto entry = getEvictionTarget(load_pc, hash); 
  if (entry == nullptr)
    return false;

  entry->tag = getTag(load_pc, hash);
  if (non_dep) {
    entry->distance = NDEP_DISTANCE;
    entry->confidence = NDEP_CONFIDENCE;
    entry->bypassCounter = NDEP_BYPASS;
  } else {
    entry->distance = static_cast<unsigned>(sq_dist);
    entry->confidence = INIT_CONFIDENCE;
    entry->bypassCounter = INIT_BYPASS;
  }

  return true;
}

MASCOT::PredictorEntry*
MASCOT::Table::getEntry(const Addr load_pc, const uint64_t hash) {
  const auto index = getIndex(load_pc, hash);
  const auto tag = getTag(load_pc, hash);

  for (auto way = 0; way < tableAssociativity; ++way) {
    if (blocks[index][way].tag == tag)
      return &(blocks[index][way]);
  }
  return nullptr;
}

MASCOT::PredictorEntry*
MASCOT::Table::getEvictionTarget(const Addr load_pc, const uint64_t hash) {
  const auto index = getIndex(load_pc, hash);

  for (auto way = 0; way < tableAssociativity; ++way) {
    if (blocks[index][way].canEvict()) {
      return &(blocks[index][way]);
    }
  }

  return nullptr;
}

} // namespace o3
} // namespace gem5
