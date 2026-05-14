/**
 * Copyright ???
 */

#include "cpu/o3/mascot.hh"

#include <cassert>
#include <cmath>
#include <cstdio>

#include "cpu/o3/mem_dep_unit.hh"
#include "params/BaseO3CPU.hh"

namespace gem5
{

namespace o3
{

using Prediction = MASCOT::Prediction;
  
static void
printStoreBuffer(const CircularQueue<MASCOT::StoreBufferEntry> &buffer) {
  if (buffer.empty()) {
    std::fprintf(stderr, "StoreBuffer: empty\n");
    return;
  }

  std::fprintf(stderr, "StoreBuffer: size=%zu\n", buffer.size());
  for (auto it = buffer.begin(); it != buffer.end(); ++it) {
    const auto &e = *it;
    std::fprintf(stderr, "  [sn:%llu] pc=%#llx isStore=%d\n",
                 static_cast<unsigned long long>(e.seq_num),
                 static_cast<unsigned long long>(e.pc),
                 static_cast<int>(e.isStore));
  }
}

MASCOT::MASCOT()
  : depCheckShift(0),
    memDepUnit(nullptr),
    histories({0, 2, 4, 8, 16, 32, 64, 128}),
    tables(histories.size(), Table()),
    storeBuffer(MAX_DISTANCE) {}

void
MASCOT::init(const BaseO3CPUParams& params, MemDepUnit* mem_dep_unit) {
  depCheckShift = params.LSQDepCheckShift;
  sqEntries = params.SQEntries;
  memDepUnit = mem_dep_unit;
}

Prediction
MASCOT::predict(Addr load_pc, InstSeqNum load_seq_num, BranchHistory branch_history) {
  const auto prediction = doPredict(load_pc, load_seq_num, branch_history);

  if (prediction.type == NDEP)
    ++(memDepUnit->stats.predictsNDep);
  else if (prediction.type == MDP)
    ++(memDepUnit->stats.predictsMDP);
  else
    ++(memDepUnit->stats.predictsSMB);

  return prediction;
}

Prediction
MASCOT::doPredict(Addr load_pc, InstSeqNum load_seq_num, BranchHistory branch_history) {
  Prediction prediction { 
    .type = NDEP, 
    .distances = {0, 0},
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
  for (long i = table_limit_idx; i >= 0; --i) {
    const uint64_t hash = generateBranchHash(histories[i], branch_history, historyBegin);
    const auto entry = tables[i].getEntry(load_pc, hash);
    if (entry == nullptr)
      continue;

    prediction.distances = entry->distances;
    prediction.tableIdx = i;
    prediction.hash = hash;
    ++(*(memDepUnit->pathReads[i])); //? Why was this pathWrites in PHAST

    // "A distance field of all 0s indicates that the entry is nondependent"
    if (entry->isNdep())
      return prediction;

    // "Whenever the distance field is not zero, a memory dependence prediction
    //  is made regardless of the value of the usefulness field, 
    prediction.type = MDP;

    //  whereas speculative memory bypassing is only predicted if both the 
    //  usefulness and bypassing counters are saturated."
    if (entry->isHighConfidence() && entry->canBypass() && entry->smbDistance() <= storeBuffer.size()) {
      auto store_idx = storeBuffer.size() - entry->smbDistance();
      prediction.type = storeBuffer[store_idx].isStore ? SMB : MDP; 

      if (prediction.type == SMB) {
        printStoreBuffer(storeBuffer);

        prediction.storeSeqNum = storeBuffer[store_idx].seq_num;
        prediction.storePC = storeBuffer[store_idx].pc;
      }
    }

    return prediction;
  }

  return prediction;
}

void 
MASCOT::commit(Addr load_pc,
                AddrSize load_addr,
                AddrSize store_addr,
                AddrSize store2_addr, // possibly empty 
                std::ptrdiff_t actual_sq_dist,
                BranchHistory branch_history,
                Prediction prediction) {
  bool misprediction;
  switch (prediction.type)
  {
  case MDP:
    // misprediction if both addrs don't overlap
    misprediction = !(addrOverlap(load_addr, store_addr,  depCheckShift) || addrOverlap(load_addr, store2_addr, depCheckShift));
    break;
  
  case SMB:
    // misprediction if addrs dont't directly match
    // This is the case where we cannot accurately get partial writes
    // when the base addresses don't match.
    misprediction = load_addr.first != store_addr.first || load_addr.second > store_addr.second;
    break;
  
  default:
    panic("NDEP type should not invoke MASCOT::commit.");
    break;
  }

  tables[prediction.tableIdx].commit(load_pc, prediction.hash, misprediction);
  ++(*(memDepUnit->pathReads[prediction.tableIdx])); // reads an entry
  ++(*(memDepUnit->pathWrites[prediction.tableIdx])); // modifies it.

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
    ++(*(memDepUnit->pathReads[prediction.tableIdx])); // reads an entry
    ++(*(memDepUnit->pathWrites[prediction.tableIdx])); // modifies it

    allocateEntry(prediction.tableIdx + 1, load_pc, branch_history, actual_sq_dist, false);
  } else {
    allocateEntry(0, load_pc, branch_history, actual_sq_dist, false);
  }

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

  ++(*(memDepUnit->pathReads[idx])); // reads for eviction target
  if (tables[idx].tryAllocate(load_pc, hash, sq_dist, non_dep, sqEntries)) {
    ++(*(memDepUnit->pathWrites[idx])); // it was successful in writing it.
    return;
  }

  tables[idx].decrConfidence(load_pc, hash);
  ++(*(memDepUnit->pathWrites[idx])); // writes to all entries in set.

  while (++idx < tables.size()) {
    hash = generateBranchHash(histories[idx], branch_history, 0);
    ++(*(memDepUnit->pathReads[idx])); // reads for eviction target
    if (tables[idx].tryAllocate(load_pc, hash, sq_dist, non_dep, sqEntries))
      ++(*(memDepUnit->pathWrites[idx])); // it was successful in writing it.
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
MASCOT::Table::tryAllocate(Addr load_pc, uint64_t hash, std::ptrdiff_t sq_dist, bool non_dep, unsigned sq_entries) {
  auto entry = getEntry(load_pc, hash);
  if (entry == nullptr) {
    // no entry exists, evict one and allocate
    auto entry = getEvictionTarget(load_pc, hash); 
    if (entry == nullptr) // can't evict in this table
      return false;

    entry->tag = getTag(load_pc, hash);
    entry->distances.first = non_dep ? NDEP_DISTANCE : sq_dist;
    entry->distances.second = NDEP_DISTANCE;
    entry->confidence = non_dep ? NDEP_CONFIDENCE : INIT_CONFIDENCE;
    entry->bypassCounter = non_dep ? NDEP_BYPASS : INIT_BYPASS;

    return true;
  }

  if (entry->distances.first != sq_dist &&
      entry->distances.second == 0 &&
      entry->distances.first < sq_entries / 2 &&
      sq_dist < sq_entries / 2) {
    entry->distances.second = sq_dist;
    entry->confidence = non_dep ? NDEP_CONFIDENCE : INIT_CONFIDENCE;
    entry->bypassCounter = non_dep ? NDEP_BYPASS : INIT_BYPASS;

    return true;
  }

  entry->distances.first = non_dep ? NDEP_DISTANCE : sq_dist;
  entry->distances.second = NDEP_DISTANCE;
  entry->confidence = non_dep ? NDEP_CONFIDENCE : INIT_CONFIDENCE;
  entry->bypassCounter = non_dep ? NDEP_BYPASS : INIT_BYPASS;

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
