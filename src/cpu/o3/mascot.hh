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

#include "base/circular_queue.hh"
#include "base/types.hh"
#include "cpu/inst_seq.hh"
#include "cpu/o3/dyn_inst_ptr.hh"

namespace gem5
{
  
struct BaseO3CPUParams;
  
namespace o3
{

#define BITSETSIZE 500

class MASCOT
{
  public:
    enum PredictionType { NDEP, MDP, SMB };

    struct Prediction {
      PredictionType type;
      unsigned distance;
      size_t tableIdx;
      uint64_t hash;
      InstSeqNum storeSeqNum; // used when type == SMB
    };

    struct PredictorEntry {
      unsigned tag = 0;           // 16 bits
      unsigned distance  = 0;     // 7 bits
      unsigned confidence = 0;    // 3 bits (usefulness)
      unsigned bypassCounter = 0; // 2 bits
    
      bool isNdep() const { return distance == NDEP_DISTANCE; }
      bool canBypass() const { return bypassCounter == MAX_BYPASS_COUNTER; } 
      bool isHighConfidence() const { return confidence == MAX_CONFIDENCE; }
      bool canEvict() const { return confidence == 0; }

      void resetCanBypass() { bypassCounter = 0; }
      void incrCanBypass() {
        if (bypassCounter < MAX_BYPASS_COUNTER){
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

    MASCOT(const BaseO3CPUParams& params) : stores(MAX_DISTANCE) { init(params); }
    ~MASCOT() {};

    void init(const BaseO3CPUParams& params);

    /**
     * Returns 
     *  * NDEP - execute load speculatively.
     *  * MDP  - stall load until store at distance executes.
     *  * SMB  - bypass the load from store at seq num.
     */
    Prediction predict(Addr load_pc, InstSeqNum load_seq_num, BranchHistory branch_history);

    /**
     * This is called when a predicted load commits successfully. This means:
     *   1. MDP was predicted but there was no need to (bad) (store addr range != load addr range).
     *   2. SMB was predicted and it was correct. (good)
     * 
     * This function checks the address range match and decides to update the entry or not.
     */
    void commit(Addr load_pc,
                std::pair<Addr, unsigned> load_addr,
                std::pair<Addr, unsigned> store_addr,
                std::ptrdiff_t actual_sq_dist,
                BranchHistory branch_history,
                Prediction prediction);

    /**
     * This is called when any load must squash. This means (all bad):
     *   1. NDEP was predicted but there was a dependence.
     *   2. MDP was predicted but it was the wrong address.
     *   3. SMB was predicted and it was wrong.
     * 
     * If there is no entry in the tables, it will be allocated.
     */
    void violation(Addr load_pc,
                  InstSeqNum store_seq_num,
                  std::ptrdiff_t actual_sq_dist,
                  Prediction prediction,
                  BranchHistory branch_history);

    void clear();

    void pushStore(InstSeqNum store_seq_num);
    void popStores(InstSeqNum squashed_seq_num);
    void removeStores(InstSeqNum seq_num);

    static constexpr std::size_t tableSize = 128;
    static constexpr std::size_t setBits = 7; // log2(tableSize)
    static constexpr std::size_t tagBits = 16;
    static constexpr std::size_t tableAssociativity = 4;

  private:
    static constexpr uint8_t MAX_DISTANCE = (1 << 7) - 1;
    static constexpr uint8_t MAX_CONFIDENCE = (1 << 3) - 1;
    static constexpr uint8_t MAX_BYPASS_COUNTER = (1 << 2) - 1;

    static constexpr uint8_t NDEP_DISTANCE = 1;
    static constexpr uint8_t NDEP_CONFIDENCE = 2;
    static constexpr uint8_t NDEP_BYPASS = 0;
    static constexpr uint8_t INIT_CONFIDENCE = 6;
    static constexpr uint8_t INIT_BYPASS = 1;

    /** Earlier the buffer, the older the stores */
    CircularQueue<InstSeqNum> stores;

    class Table
    {
      public:
        Table() {
          blocks.resize(tableSize);

          for (auto& block : blocks)
            block.resize(tableAssociativity);
        }

        /**
         * @return entry matching index and tag, nullptr if not found.
         */
        PredictorEntry* getEntry(const Addr load_pc, const uint64_t hash);

        /**
         * @return the entry matching index and tag, or the lowest the confidence entry
         */
        PredictorEntry* getEvictionTarget(const Addr load_pc, const uint64_t hash);

        /**
         * Invoke this function to change the counters in the entry.
         * @return false if the entry is not found.
         */
        void commit(Addr load_pc, uint64_t hash, bool misprediction);

        bool tryAllocate(Addr load_pc, uint64_t hash, std::ptrdiff_t sq_dist, bool non_dep);

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
          for (auto& block : blocks)
            block.clear();
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
    
    std::vector<unsigned> histories;
    std::vector<Table> tables;

    unsigned depCheckShift;

    void allocateEntry(const unsigned startTableIdx,
                      Addr load_pc,
                      BranchHistory branch_history,
                      std::ptrdiff_t sq_dist,
                      bool non_dep);

    uint64_t generateBranchHash(unsigned num_branches,
                                BranchHistory branch_history,
                                size_t start_idx) {
      const auto end_idx = start_idx + num_branches;
      if (end_idx >= branch_history.size())
        return 0;

      constexpr unsigned targetBits = 5;
      const unsigned targetMask = (1 << targetBits) - 1;

      std::bitset<BITSETSIZE> h = branch_history[end_idx].target & targetMask;
      
      unsigned bits = 60;
      unsigned items = 0;
      for (unsigned idx = end_idx - 1; idx >= start_idx && items < num_branches; --idx) {
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

};
  
} // namespace o3
} // namespace gem5

#endif