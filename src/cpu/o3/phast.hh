/*
 * Copyright (c) 2004-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __CPU_O3_PHAST_HH__
#define __CPU_O3_PHAST_HH__

#include "base/statistics.hh"
#include "base/types.hh"
#include "cpu/inst_seq.hh"
#include "cpu/o3/dyn_inst_ptr.hh"
#include "cpu/o3/limits.hh"
#include "debug/MemDepUnit.hh"
#include "dyn_inst_ptr.hh"
#include "mem/packet.hh"
#include "mem/port.hh"
#include <cstdint>
#include <vector>
#include <deque>
#include <bitset>

using namespace std;

namespace gem5
{

namespace o3
{

struct MemDepInfo {
    /** Store this load received its data from, if any */
    InstSeqNum forwardedFrom = 0;
    /** Youngest store this load violated with */
    InstSeqNum violatingStoreSeqNum;
    /** Relative offset into the SQ for dependent store*/
    std::ptrdiff_t storeQueueDistance;
    /** Memory location of store this load was predicted dependent on */
    Addr predStoreAddr = 0;
    int predStoreSize;
    /** Predicted information validated at commit */
    unsigned predBranchHistLength;
    uint64_t predictorHash;
};

struct PredictionResult {
    std::ptrdiff_t storeQueueDistance;
    unsigned predBranchHistLength;
    uint64_t predictorHash;
};

#define BITSETSIZE 500

class MemDepUnit;

class PHAST
{

  public:

    class SimplBlockCache;

    /** Default constructor.  init() must be called prior to use. */
    PHAST() { };

    /** Creates PHAST predictor with given table sizes. */
    PHAST(uint64_t num_rows, uint64_t associativity, uint64_t tag_bits, uint64_t max_counter_value, MemDepUnit *mem_dep_unit);

    /** Default destructor. */
    ~PHAST();

    /** Initializes the PHAST predictor with the given table sizes. */
    void init(uint64_t num_rows, uint64_t associativity, uint64_t tag_bits, uint64_t max_counter_value, MemDepUnit *mem_dep_unit);

    /** Records a memory ordering violation between the younger load
    * and the older store. */
    void violation(Addr load_pc, InstSeqNum store_seq_num, std::ptrdiff_t storeQueueDistance, BranchHistory branchHistory);

    /** Checks if the instruction with the given PC is dependent upon
    * any store.  @return Returns the relative SQ distance of the store
    * instruction this PC is dependent upon.  Returns -1 if none.
    */
    PredictionResult checkInst(Addr load_pc, InstSeqNum load_seq_num, BranchHistory branchHistory);

    /** Updates predictor at load commit */
    void commit(Addr load_pc, Addr load_addr, unsigned load_size, Addr store_addr, unsigned store_size, unsigned path_index, uint64_t predictor_hash);

    /** Clears all tables */
    void clear();

    /** mem_dep_unit interface methods that don't do anything in PHAST */
    void squash(InstSeqNum squashed_num, ThreadID tid);
    void issued(Addr issued_PC, InstSeqNum issued_seq_num, bool is_store) { return; }
    void insertStore(Addr store_PC, InstSeqNum store_seq_num, ThreadID tid) { return; }
    void insertLoad(Addr load_PC, InstSeqNum load_seq_num) { return;}

    unsigned selectedTargetBits;

    uint64_t selectedTargetMask;

  private:

    bool debug;

    //largest seen index into branchSizes
    unsigned maxBranches;

    std::vector<unsigned> historySizes;

    std::vector<SimplBlockCache> paths;

    unsigned maxHistory;

    unsigned entriesPerTable;

    MemDepUnit *memDepUnit;

    uint64_t generateBranchHash(unsigned path_index, unsigned num_branches, BranchHistory::iterator branchHistoryBegin, BranchHistory::iterator branchHistoryEnd);

    uint64_t foldHistory(std::bitset<BITSETSIZE> h, int bits, unsigned _set_bits, unsigned _tag_bits);

    public:

    class SimplBlockCache {
        struct Entry {
            uint64_t tag;
            std::ptrdiff_t distance;
            uint32_t lru;
            uint32_t counter;
        };

        uint32_t setBits;
        uint32_t tagBits;
        uint32_t associativity;
        uint64_t lruCounter;
        unsigned maxCounterValue;
        std::vector<std::vector<Entry>> cache;

        uint64_t xorFold(uint64_t pc, uint64_t history, unsigned size) const;

        uint64_t getIndex(Addr pc, uint64_t history) const;

        uint64_t getTag(Addr pc, uint64_t history) const;

        Entry* findEntry(Addr pc, uint64_t history);

        Entry* getLRUEntry(uint64_t set);

        void updateLRU(Entry* entry);

        public:
            int init(uint32_t set_bits, uint32_t _associativity, uint32_t tag_bits, uint32_t max_counter_value);

            std::ptrdiff_t predict(Addr pc, uint64_t history);

            void update(Addr pc, uint64_t history, std::ptrdiff_t distance);

            void updateCommit(Addr pc, uint64_t history, bool predictionWrong);

            void clear();


            unsigned getSetBits() { return setBits; }

            unsigned getTagBits() { return tagBits; }
    };

};

} // namespace o3
} // namespace gem5

#endif // __CPU_O3_PHAST_HH__
