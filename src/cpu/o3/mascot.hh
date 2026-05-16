/**
 * Copyright ???
 */

#ifndef __CPU_O3_MASCOT_HH__
#define __CPU_O3_MASCOT_HH__

#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "base/types.hh"
#include "cpu/inst_seq.hh"
#include "cpu/o3/dyn_inst_ptr.hh"
#include "cpu/o3/utils.hh"

namespace gem5 {

struct BaseO3CPUParams;

namespace o3 {

class MemDepUnit;

#define BITSETSIZE 500

class MASCOT {
public:
  enum PredictionType { NDEP, MDP, SMB };

  struct Prediction {
    PredictionType type;
    std::pair<std::ptrdiff_t, std::ptrdiff_t> distances;
    size_t tableIdx;
    uint64_t hash;
    bool fromTable;
  };

  struct PredictorEntry {
    bool valid;
    uint16_t tag = 0;                                             // 16 bits
    std::pair<std::ptrdiff_t, std::ptrdiff_t> distances = {0, 0}; // each 7 bits
    uint8_t confidence = 0;    // 3 bits (usefulness)
    uint8_t bypassCounter = 0; // 2 bits

    bool isNdep() const {
      return distances.first == NDEP_DISTANCE &&
             distances.second == NDEP_DISTANCE;
    }
    /** true if counter is saturated and we have exactly one distance value */
    bool canBypass() const {
      return bypassCounter == MAX_BYPASS_COUNTER &&
             (distances.first == NDEP_DISTANCE ^
              distances.second == NDEP_DISTANCE);
    }
    bool isHighConfidence() const { return confidence == MAX_CONFIDENCE; }
    bool canEvict() const { return !valid || confidence == 0; }

    void resetCanBypass() { bypassCounter = 0; }
    void incrCanBypass() {
      if (bypassCounter < MAX_BYPASS_COUNTER) {
        ++bypassCounter;
      }
    }

    void decrConfidence() {
      if (confidence > 0) {
        --confidence;
      }
    }
    void incrConfidence() {
      if (confidence < MAX_CONFIDENCE) {
        ++confidence;
      }
    }
  };

  MASCOT();
  MASCOT(const BaseO3CPUParams &params, MemDepUnit *mem_dep_unit) {
    init(params, mem_dep_unit);
  }

  ~MASCOT() {};

  void init(const BaseO3CPUParams &params, MemDepUnit *mem_dep_unit);

  MemDepUnit *memDepUnit;

  /**
   * Returns
   *  * NDEP - execute load speculatively.
   *  * MDP  - stall load until store at distance executes.
   *  * SMB  - bypass the load from store at seq num.
   */
  Prediction predict(Addr load_pc, InstSeqNum load_seq_num,
                     BranchHistory branch_history);

  /**
   * This is called when a predicted load commits successfully. This means:
   *   1. MDP was predicted but there was no need to (bad) (store addr range !=
   * load addr range).
   *   2. SMB was predicted and it was correct. (good)
   *
   * This function checks the address range match and decides to update the
   * entry or not.
   */
  void commit(Addr load_pc, AddrSize load_addr, AddrSize store_addr,
              AddrSize store2_addr, std::ptrdiff_t actual_sq_dist,
              BranchHistory branch_history, Prediction prediction);

  /**
   * This is called when any load must squash. This means (all bad):
   *   1. NDEP was predicted but there was a dependence.
   *   2. MDP was predicted but it was the wrong address.
   *   3. SMB was predicted and it was wrong.
   *
   * If there is no entry in the tables, it will be allocated.
   */
  void violation(Addr load_pc, InstSeqNum store_seq_num,
                 std::ptrdiff_t actual_sq_dist, Prediction prediction,
                 BranchHistory branch_history);

  void clear();

  static constexpr std::size_t tableSize = 128;
  static constexpr std::size_t setBits = 7; // log2(tableSize)
  static constexpr std::size_t tagBits = 16;
  static constexpr std::size_t tableAssociativity = 4;

private:
  static constexpr uint8_t MAX_DISTANCE = (1 << 7) - 1;
  static constexpr uint8_t MAX_CONFIDENCE = (1 << 3) - 1;
  static constexpr uint8_t MAX_BYPASS_COUNTER = (1 << 2) - 1;

  static constexpr uint8_t NDEP_DISTANCE = 0;
  static constexpr uint8_t NDEP_CONFIDENCE = 2;
  static constexpr uint8_t NDEP_BYPASS = 0;
  static constexpr uint8_t INIT_CONFIDENCE = 6;
  static constexpr uint8_t INIT_BYPASS = 1;

  static constexpr size_t BASE_PREDICTOR_IDX = -1ul;

  class Table {
  public:
    Table() {
      blocks.resize(tableSize);

      for (auto &block : blocks)
        block.resize(tableAssociativity, PredictorEntry());
    }

    /**
     * @return entry matching index and tag, nullptr if not found.
     */
    PredictorEntry *getEntry(const Addr load_pc, const uint64_t hash);

    /**
     * @return the entry matching index and tag, or the lowest the confidence
     * entry
     */
    PredictorEntry *getEvictionTarget(const Addr load_pc, const uint64_t hash);

    /**
     * Invoke this function to change the counters in the entry.
     * @return false if the entry is not found.
     */
    void commit(Addr load_pc, uint64_t hash, bool misprediction, bool should_bypass);

    bool tryAllocate(Addr load_pc, uint64_t hash, std::ptrdiff_t sq_dist,
                     bool non_dep, bool should_bypass, unsigned sq_entries);

    void decrConfidence(Addr load_pc, uint64_t hash) {
      const auto index = getIndex(load_pc, hash);

      for (auto way = 0; way < tableAssociativity; ++way) {
        blocks[index][way].decrConfidence();
      }
    }

    uint64_t getIndex(Addr pc, uint64_t history) const {
      auto _pc = (pc ^ (pc >> 2) ^ (pc >> 5));
      return xorFold(0, (_pc ^ history), setBits);
    }

    uint64_t getTag(Addr pc, uint64_t history) const {
      auto _pc = (pc ^ (pc >> 3) ^ (pc >> 7));
      return xorFold(0, (_pc ^ history), tagBits);
    }

    void clear() {
      for (auto &block : blocks)
        for (auto &entry : block)
          entry = PredictorEntry();
    }

  private:
    std::vector<std::vector<PredictorEntry>> blocks;

    uint64_t xorFold(Addr pc, uint64_t history, size_t size) const {
      uint64_t mask = (1 << size) - 1;
      uint64_t fold = (history & mask);
      fold = (fold ^ (pc & mask));

      history = (history >> size);

      while (history) {
        fold = (fold ^ (history & mask));
        history = (history >> size);
      }

      return fold;
    }
  };

  unsigned depCheckShift;
  unsigned sqEntries;

  std::vector<unsigned> histories;
  std::vector<Table> tables;

  Prediction doPredict(Addr load_pc, InstSeqNum load_seq_num,
                       BranchHistory branch_history);

  void allocateEntry(const unsigned startTableIdx, Addr load_pc,
                     BranchHistory branch_history, std::ptrdiff_t sq_dist,
                     bool non_dep, bool should_bypass);

  uint64_t foldHistory(std::bitset<BITSETSIZE> h, unsigned bits) {
    auto width = setBits + tagBits;
    std::bitset<BITSETSIZE> mask((1ULL << width) - 1);
    uint64_t hash = 0;

    while (bits >= width) {
      hash ^= (h & mask).to_ullong();
      h >>= width;
      bits -= width;
    }

    hash ^= (h & std::bitset<BITSETSIZE>((1ULL << bits) - 1)).to_ullong();

    return hash;
  }

  uint64_t generateBranchHash(unsigned num_branches,
                              BranchHistory branch_history, size_t start_idx) {
    const auto end_idx = start_idx + num_branches;
    if (end_idx >= branch_history.size())
      return 0;

    constexpr unsigned targetBits = 5;
    const unsigned targetMask = (1 << targetBits) - 1;

    std::bitset<BITSETSIZE> h = branch_history[end_idx].target & targetMask;

    unsigned bits = 60;
    unsigned items = 0;
    for (long idx = end_idx - 1; idx >= start_idx && items < num_branches;
         --idx) {
      if (!branch_history[idx].indirect) {
        h <<= 1;
        h[0] = branch_history[idx].taken;
        ++items;
        ++bits;
      } else if (targetMask != 0) {
        h <<= targetBits;
        h ^= branch_history[idx].target & targetMask;
        ++items;
        bits += targetBits;
      }
    }

    return foldHistory(h, bits);
  }
};

struct MascotInfo {
  MASCOT::Prediction prediction;

  InstSeqNum violatingStoreSeqNum = 0;
  Addr violatingStorePC = 0;
  AddrSize predStoreAddr = {0, 0};
  AddrSize predStore2Addr = {0, 0};
  std::ptrdiff_t actualSQDist = 0;

  bool smbPredicted = false;
  bool smbViolation = false;
  InstSeqNum smbStoreSeqNum = 0;

  InstSeqNum mdpForwardedFrom = 0;
  bool mdpViolation = false;
  bool mdpPredicted = false;

  bool violation() const { return smbViolation || mdpViolation; }
  bool predicted() const { return smbPredicted || mdpPredicted; }
};

} // namespace o3
} // namespace gem5

#endif
