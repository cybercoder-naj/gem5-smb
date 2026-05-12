/**
 * Copyright ???
 */

#include "cpu/o3/mascot.hh"

#include <cassert>
#include <cmath>

#include "params/BaseO3CPU.hh"

namespace gem5
{

namespace o3
{

using Prediction = MASCOT::Prediction;
  
void MASCOT::init(const BaseO3CPUParams& params) {
  depCheckShift = params.LSQDepCheckShift;

  histories.assign({0, 2, 4, 8, 16, 32, 64, 128});
  tables.resize(histories.size(), Table());
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

  for (size_t i = tables.size() - 1; i >= 0; --i) {
    const uint64_t hash = generateBranchHash(histories[i], branch_history, i);
    const auto entry = tables[i].getEntry(load_pc, hash);
    if (entry != nullptr && (entry->distance != 0 || entry->isNdep())) {
      prediction.distance = entry->distance;
      prediction.tableIdx = i;
      prediction.hash = hash;

      if (entry->isHighConfidence()) {
        prediction.type = entry->canBypass() ? SMB : MDP;
        return prediction;
      } 
    }
  }

  return prediction;
}

void 
MASCOT::commit(Addr load_pc,
                Addr load_addr,
                unsigned load_size,
                Addr store_addr,
                unsigned store_size,
                BranchHistory branch_history,
                std::ptrdiff_t sq_dist,
                Prediction prediction) {
  Addr load_addr_start = load_addr >> depCheckShift;
  Addr load_addr_end = (load_addr + load_size - 1) >> depCheckShift;

  Addr store_addr_start = store_addr >> depCheckShift;
  Addr store_addr_end = (store_addr + store_size - 1) >> depCheckShift;

  // misprediction == true iff MDP/SMB was predicted but it didn't have to.
  bool misprediction = !(load_addr_start <= store_addr_end && store_addr_start <= load_addr_end);

  tables[prediction.tableIdx].commit(load_pc, prediction.hash, misprediction);

  if (misprediction) {
    // Allocate non dependency in next table.
    allocateEntry(prediction.tableIdx + 1, load_pc, branch_history, sq_dist, true);
  }
}

void
MASCOT::violation(Addr load_pc,
                  InstSeqNum store_seq_num,
                  std::ptrdiff_t sq_dist,
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

  //quantise num branches to first lowest path size
  unsigned tableIndex = 0;
  if (actual_branches >= histories.back()) {
    tableIndex = histories.size() - 1;
  } else {
    for (unsigned i = 1; i < histories.size(); ++i) {
      if (actual_branches < histories[i]) {
        tableIndex = i - 1;
        break;
      }
    }
  }
  const auto historySize = histories[tableIndex];

  auto hash = generateBranchHash(historySize, branch_history, 0);
  tables[prediction.tableIdx].commit(load_pc, prediction.hash, true);
  allocateEntry(prediction.tableIdx + 1, load_pc, branch_history, sq_dist, false);
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
