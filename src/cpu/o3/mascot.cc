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

MASCOT::MASCOT()
  : depCheckShift(0),
    memDepUnit(nullptr),
    histories({0, 2, 4, 8, 16, 32, 64, 128}),
    tables(histories.size(), Table()) {}

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
    .tableIdx = BASE_PREDICTOR_IDX,
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
  if (historyBegin == branch_history.size()) return prediction; //no +1 branch

  auto table_limit_idx = 0;
  while (table_limit_idx + 1 < histories.size() &&
         histories[table_limit_idx + 1] < branch_history.size()) { 
    ++table_limit_idx; 
  }

  // "The tables are searched in parallel, and the entry from the highest-context table that has a match is used for prediction."
  for (long i = table_limit_idx; i >= 0; --i) {
    const uint64_t hash = generateBranchHash(histories[i], branch_history, historyBegin);
    const auto entry = tables[i].getEntry(load_pc, hash);
    if (entry == nullptr)
      // When the predictor finds no matching entry it will default to predicting a non-dependency (termed the base predictor)
      continue;

    prediction.distances = entry->distances;
    prediction.tableIdx = i;
    prediction.hash = hash;

    // "A distance field of all 0s indicates that the entry is nondependent"
    if (entry->isNdep())
      return prediction;

    // "Whenever the distance field is not zero, a memory dependence prediction
    //  is made regardless of the value of the usefulness field, 
    //  whereas speculative memory bypassing is only predicted if both the 
    //  usefulness and bypassing counters are saturated."
    prediction.type = entry->isHighConfidence() && entry->canBypass() ? SMB : MDP;

    return prediction;
  }

  return prediction;
}

void 
MASCOT::commit(Addr load_pc,
                AddrSize load_addr,
                AddrSize store_addr,
                AddrSize store2_addr,
                std::ptrdiff_t actual_sq_dist,
                BranchHistory branch_history,
                Prediction prediction) {
  bool misprediction = false;

  switch (prediction.type)
  {
  case MDP:
    // misprediction if both addrs don't overlap
    misprediction = !(addrOverlap(load_addr, store_addr,  depCheckShift) || addrOverlap(load_addr, store2_addr, depCheckShift));
    break;
  
  case SMB:
    // Must not be empty!
    assert(store_addr.first != 0);
    // Must be empty!
    assert(store2_addr.first == 0);
    // Misprediction if base address don't match or load asked for longer than store
    misprediction = load_addr.first != store_addr.first || load_addr.second > store_addr.second;
    break;
  
  default:
    panic("NDEP type should not invoke MASCOT::commit.");
    break;
  }

  tables[prediction.tableIdx].commit(load_pc, prediction.hash, misprediction);
  ++(*(memDepUnit->pathReads[prediction.tableIdx])); // reads an entry
  ++(*(memDepUnit->pathWrites[prediction.tableIdx])); // modifies it.

  // "There are three types of misprediction that will lead to an allocation
  //  in a table with a longer history... ...
  //  Finally, when a load is predicted to depend on a prior store but no conflicts are detected."
  if (misprediction) {
    // Allocate non dependency in next table.
    allocateEntry(prediction.tableIdx + 1, load_pc, branch_history, actual_sq_dist, true);
    if (prediction.type == MDP) {
      ++(memDepUnit->stats.falseDependencies);
      ++(memDepUnit->stats.mascotMDPMispredictions);
    } else {
      ++(memDepUnit->stats.mascotSMBMispredictions);
    }
  } else {
    ++(memDepUnit->stats.correctPredictions);
  }
}

void
MASCOT::violation(Addr load_pc,
                  InstSeqNum store_seq_num,
                  std::ptrdiff_t actual_sq_dist,
                  Prediction prediction,
                  BranchHistory branch_history) {
  if (prediction.tableIdx != BASE_PREDICTOR_IDX) {
    // The prediction came from a table in MASCOT. 
    // update counters for next hash.
    tables[prediction.tableIdx].commit(load_pc, prediction.hash, true);
    ++(*(memDepUnit->pathReads[prediction.tableIdx])); // reads an entry
    ++(*(memDepUnit->pathWrites[prediction.tableIdx])); // modifies it
  }

  if (prediction.type == NDEP) {
    ++(memDepUnit->stats.mascotNDepMispredictions);
  } else if (prediction.type == MDP) {
    ++(memDepUnit->stats.mascotMDPMispredictions);
  } else {
    ++(memDepUnit->stats.mascotSMBMispredictions);
  }

  // "There are three types of misprediction that will lead to an allocation
  //  in a table with a longer history. First, when a load is predicted not to
  //  depend on any prior store but does have a dependence on one. Second,
  //  when a load is predicted to  depend on a particular prior store but actually
  //  conflicts with a different one (where either the predicted store has the
  //  wrong address, or the conflict is with a younger store).""
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
  // This is called from violation/commit only,
  // where we pass the 'committedBranchHistory'
  // so index = 0 is the youngest branch older than
  // the load. so 0 is fine here.
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
    if (tables[idx].tryAllocate(load_pc, hash, sq_dist, non_dep, sqEntries)) {
      ++(*(memDepUnit->pathWrites[idx])); // it was successful in writing it.
      return;
    }
  }
}

void
MASCOT::clear() {
  for (auto& table : tables) {
    table.clear();
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
