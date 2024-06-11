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

#include "base/types.hh"
#include "cpu.hh"
#include "cpu/inst_seq.hh"

using namespace std;

namespace gem5
{

namespace o3
{

#define BITSETSIZE 500

class SimplBlockCache;

class PHAST
{

  public:
    /** Default constructor.  init() must be called prior to use. */
    PHAST() { };

    /** Creates PHAST predictor with given table sizes. */
    PHAST(uint64_t_t max_history_length, uint64_t_t entries_per_table, uint64_t_t set_bits, uint64_t_t tag_bits, uint64_t_t max_counter_value, uint64_t_t associativity);

    /** Default destructor. */
    ~PHAST();

    /** Initializes the PHAST predictor with the given table sizes. */
    void init(uint64_t_t max_history_length, uint64_t_t entries_per_table, uint64_t_t set_bits, uint64_t_t tag_bits, uint64_t_t max_counter_value, uint64_t_t associativity);

    /** Records a memory ordering violation between the younger load
    * and the older store. */
    void violation(DynInstPtr store, DynInstPtr load, BranchHistory branchHistory);

    /** Inserts a load into the PHAST predictor.  This does nothing but
    * is included in case other predictors require a similar function.
    */
    void insertLoad(Addr load_PC, InstSeqNum load_seq_num);

    /** Inserts a store into the PHAST predictor.  Updates the
    * LFST if the store has a valid SSID. */
    void insertStore(Addr store_PC, InstSeqNum store_seq_num, ThreadID tid);

    /** Checks if the instruction with the given PC is dependent upon
    * any store.  @return Returns the sequence number of the store
    * instruction this PC is dependent upon.  Returns 0 if none.
    */
    InstSeqNum checkInst(DynInstPtr load, BranchHistory branchHistory);

    /** Records this PC/sequence number as issued. */
    void issued(Addr issued_PC, InstSeqNum issued_seq_num, bool is_store);

    //TODO: add to memdep interface
    void commit_load(Addr load_pc, InstSeqNum load_seq_num, Addr store_pc, bool is_dspec = true, bool has_stalled = true, LoadExecutionType exec_type = LoadExecutionType::NotSpec);

  private:

    std::vector<unsigned> history_sizes;

    std::vector<SimplBlockCache> paths;

    unsigned maxHistory;

    unsigned entriesPerTable;

    enum LoadExecutionType {
        NotSpec,
        FalseDependence,
        Conflictive,
    };

    uint64_t generateBranchHash(unsigned begin_dist, unsigned num_branches, unsigned index);

    uint64_t foldHistory(std::bitset<BITSETSIZE> h, int bits, unsigned _set_bits, unsigned _tag_bits);

    class SimplBlockCache {
        struct ENTRY {
            uint64_t_t tag;
            InstSeqNum dep_store;
            uint32_t_t lru;
            uint32_t_t counter;
        };

        uint32_t_t SET_BITS;
        uint32_t_t TAG_BITS;
        uint32_t_t WAYS;
        //table has (1 << SET_BITS) sets which each have WAYS slots (total entries = sets * ways)
        std::vector<std::vector<ENTRY>> cache;
        uint64_t_t lru__counter;
        unsigned max_counter_value;

        uint64_t xorFold(uint64_t pc, uint64_t history, unsigned size) const;

        uint64_t getIndex(Addr pc, uint64_t history) const;

        uint64_t getTag(Addr pc, uint64_t history) const;

        ENTRY* findEntry(Addr pc, uint64_t history);

        ENTRY* getLRUEntry(uint64_t set);

        void updateLRU(ENTRY* entry);

        public:
            int init(unsigned max_ctr, unsigned set_bits, unsigned tag_bits, unsigned ways);

            uint32_t predict(Addr pc, uint64_t history);

            void update(Addr pc, uint64_t history, InstSeqNum dep_store);

            void updateCommit(Addr pc, uint64_t history, bool predictionWrong);

            unsigned getSetBits() { return SET_BITS; }

            unsigned getTagBits() { return TAG_BITS; }
    };

};

} // namespace o3
} // namespace gem5

#endif // __CPU_O3_PHAST_HH__
