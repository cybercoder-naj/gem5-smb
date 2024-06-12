/*
 * Copyright (c) 2004-2006 The Regents of The University of Michigan
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

#include "cpu/o3/phast.hh"

#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include <cmath>

namespace gem5
{

namespace o3
{

PHAST::PHAST(uint64_t max_history_length, uint64_t entries_per_table, uint64_t set_bits, uint64_t tag_bits, uint64_t associativity) {

    assert(!isPowerOf2(max_history_length) && "Invalid max history length!\n");

    historySizes.push_back(0);
    for (unsigned i=2; i <= max_history_length; i <<= 1)
        historySizes.push_back(i);

    //TODO
    maxBranches = 0;
    //FIXME: parametise this
    SELECTED_TARGET_MASK = (1 << ) - 1;
    //FIXME: parametise this
    MAX_COUNTER = (1 << ) - 1;

    num_tables = historySizes.size();
    paths = std::vector<SimplBlockCache>();
    paths.reserve(num_tables);

    unsigned num_entries = 0;
    for (unsigned i = 0; i < num_tables; ++i) {
        num_entries += paths[i].init(max_counter_value, set_bits, tag_bits, associativity);
    }

}


PHAST::~PHAST()
{
}

void insertLoad(Addr load_PC, InstSeqNum load_seq_num) {
    return;
}

void insertStore(Addr store_PC, InstSeqNum store_seq_num, ThreadID tid) {
}

ssize_t PHAST::checkInst(DynInstPtr load, BranchHistory branchHistory) {
    ssize_t sq_idx = -1;

    auto begin = branchHistory.begin();
    InstSeqNum branch_seq_num = load->seqNum;
    while (begin != branchHistory.end() && load->seqNum > branch_seq_num) {
        branch_seq_num = begin->seqNum;
        begin++;
    }

    uint64_t hash;
    ssize_t tmp_idx;
    for (unsigned i = 0; i <= maxBranches && i < historySizes.size(); i++) {
        hash = generateBranchHash(historySizes[i], i, begin);
        tmp_idx = paths[i].predict(load->pcState()->instAddr(), hash);
        if (tmp_idx != -1) {
            sq_idx = tmp_idx
        }
    }

    return sq_idx;
}

void PHAST::violation(DynInstPtr store, DynInstPtr load, BranchHistory branchHistory) {
    uint64_t first_jump = 0;
    uint64_t path_hash = 0;
    unsigned hist_items = 0;
    bitset<BITSETSIZE> h(0);
    int bits = 60;
    int hs_idx = 0;

    //corner case of a violation before any branches
    if (branchHistory.empty()) return;

    //branch history passed from commit, so we know the first branch will
    //be older than the load. just need to search for the store.
    auto br_it = branchHistory.begin();
    InstSeqNum branch_seq_num;
    do {
        branch_seq_num = br_it->seqNum;
        br_it++;
    } while (br_it != branchHistory.end() && store->seqNum > branch_seq_num);

    unsigned num_branches = (unsigned)std::distance(branchHistory.begin(), br_it);

    //truncate num branches to first power of two
    unsigned path_index = 0;
    for (unsigned i = historySizes.size(); i-- > 0;) {
        unsigned size = historySizes[i];
        if ((num_branches & size) == size) {
            path_index = size;
            break;
        }
    }

    path_hash = generateBranchHash(num_branches, path_index, branchHistory.begin());

    paths[path_index].update(load->pcState().instAddr(), path_hash, store_distance);
    maxBranches = max(maxBranches, path_index);
}

uint64_t PHAST::generateBranchHash(unsigned num_branches, unsigned path_index, BranchHistory::iterator branchHistory) {
    deque<uint64_t> tmp_path;
    tmp_path.clear();
    int bits = 60;
    bitset<BITSETSIZE> h = branchHistory[num_branches].target & SELECTED_TARGET_MASK;  // This is the +1 branch.
    tmp_path.push_back(branchHistory[num_branches].target);

    //TODO: does reversing the order change the hash?
    unsigned hist_items = 0;
    for (auto pos = branchHistory; pos != branchHistory.end() && hist_items < num_branches; pos++) {
        if (!branchHistory[pos].indirect) {
            h <<= 1;
            h[0] = branchHistory[pos].taken;
            ++hist_items;
            ++bits;
            tmp_path.push_back(branchHistory[pos].taken);
        } else if (SELECTED_TARGET_MASK != 0) {
            h <<= SELECTED_TARGET_BITS;
            h ^= (branchHistory[pos].target & SELECTED_TARGET_MASK);
            ++hist_items;
            bits += SELECTED_TARGET_BITS;
            tmp_path.push_back(branchHistory[pos].target);
        }
    }

    return foldHistory(h, bits, paths[index].getSetBits(), paths[index].getTagBits());
}

uint64_t PHAST::foldHistory(bitset<BITSETSIZE> h, int bits, unsigned _setBits, unsigned _tagBits) {
    int width = _setBits + _tagBits;
    bitset<BITSETSIZE> mask((1ULL << width) - 1);
    uint64 hash = 0;

    while (bits >= width) {
        hash ^= (h & mask).to_ullong();
        h >>= width;
        bits -= width;
    }

    hash ^= (h & bitset<BITSETSIZE>((1ULL << bits) - 1)).to_ullong();

    return hash;
}

int PHAST::SimplBlockCache::init(unsigned max_ctr, unsigned set_bits, unsigned tag_bits, unsigned _ways) {
    tagBits = tag_bits;
    setBits = set_bits;
    ways = _ways;
    lruCounter = 0;
    maxCounterValue = max_ctr;

    cache = std::vector<std::vector<ENTRY>>(1 << SETBITS);

    for (uint64_t i = 0; i < (1ULL << SETBITS); i++) {
        cache[i] = vector<ENTRY>(WAYS);

        for (uint32_t j = 0; j < WAYS; j++) {
            cache[i][j].tag = 0;
            cache[i][j].sq_idx = -1;
            cache[i][j].lru = 0;
            cache[i][j].counter = 0;
        }
    }
    return (1 << SETBITS) * WAYS;
}

uint64_t PHAST::SimplBlockCache::xorFold(uint64_t pc, uint64_t history, unsigned size) const {
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

uint64_t PHAST::SimplBlockCache::getIndex(Addr pc, uint64_t history) const {
    uint64_t ldPC = pc.getAddress();
    ldPC = (ldPC ^ (ldPC >> 2) ^ (ldPC >> 5));
    uint64_t index = xorFold(0, (ldPC ^ history), SETBITS);
    return index;
}
uint64_t PHAST::SimplBlockCache::getTag(Addr pc, uint64_t history) const {
    uint64_t ldPC = pc.getAddress();
    ldPC = (ldPC ^ (ldPC >> 3) ^ (ldPC >> 7));
    uint64_t tag = xorFold(0, (ldPC ^ history), TAGBITS);
    return tag;
}

ENTRY* PHAST::SimplBlockCache::findEntry(Addr pc, uint64_t history) {
    uint64_t set = getIndex(pc, history);
    uint64_t tag = getTag(pc, history);
    for (uint32_t i = 0; i < WAYS; i++) {
        if (cache[set][i].tag == tag) {
            return &(cache[set][i]);
        }
    }
    return nullptr;
}

ENTRY* PHAST::SimplBlockCache::getLRUEntry(uint64_t set) {
    uint32_t lru_way = 0;
    uint64_t lru_value = cache[set][lru_way].lru;
    for (uint32_t i = 0; i < WAYS; i++) {
        if (cache[set][i].lru < lru_value) {
            lru_way = i;
            lru_value = cache[set][lru_way].lru;
        }
    }
    return &(cache[set][lru_way]);
}

void PHAST::SimplBlockCache::updateLRU(ENTRY* entry) {
    entry->lru = lru_counter;
    lru_counter++;
}

ssize_t PHAST::SimplBlockCache::predict(Addr pc, uint64_t history) {
    auto entry = findEntry(pc, history);
    if (entry == nullptr || entry->counter == 0 || entry->sq_idx == -1) { // no prediction for this PC
        return 0;
    }

    updateLRU(entry);

    return entry->sq_idx;
}

void PHAST::SimplBlockCache::update(Addr pc, uint64_t history, ssize_t sq_idx) {
    auto entry = findEntry(pc, history);
    if (entry == nullptr) {
        // no prediction for this entry so far, so allocate one
        entry = getLRUEntry(getIndex(pc, history));
        entry->tag = getTag(pc, history);
        entry->sq_idx = sq_idx;
        entry->counter = max_counter_value;
        updateLRU(entry);
    } else {
        entry->sq_idx = sq_idx;
        entry->counter = max_counter_value;
        updateLRU(entry);
    }
}

void PHAST::SimplBlockCache::updateCommit(Addr pc, uint64_t history, bool predictionWrong) {
    auto entry = findEntry(pc, history);
    if (entry == NULL || entry->counter == 0) {
        return;
    }

    if (predictionWrong) {
        if (entry->counter > 0) {
            --entry->counter;
        }
    } else {
        entry->counter = max_counter_value;
    }

    updateLRU(entry);
}

} // namespace o3
} // namespace gem5
