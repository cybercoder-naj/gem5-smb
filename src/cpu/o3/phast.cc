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

PHAST::PHAST(uint64_t max_history_length, uint64_t num_rows, uint64_t associativity, uint64_t tag_bits) {

    assert(isPowerOf2(max_history_length) && "Invalid max history length!\n");
    assert(isPowerOf2(num_rows) && "Invalid number of rows per table!\n");

    historySizes.push_back(0);
    for (unsigned i=2; i <= max_history_length; i <<= 1)
        historySizes.push_back(i);

    maxBranches = 0;
    selectedTargetBits = 5;
    selectedTargetMask = (1 << selectedTargetBits) - 1;

    unsigned set_bits = log((double)num_rows, 2);

    num_tables = historySizes.size();
    paths = std::vector<SimplBlockCache>();
    paths.reserve(num_tables);

    for (unsigned i = 0; i < num_tables; ++i) {
        paths[i].init(counter_bits, set_bits, tag_bits, associativity);
    }

}

PHAST::~PHAST()
{
}

PHAST::init(uint64_t max_history_length, uint64_t num_rows, uint64_t associativity, uint64_t tag_bits) {

    assert(isPowerOf2(max_history_length) && "Invalid max history length!\n");
    assert(isPowerOf2(num_rows) && "Invalid number of rows per table!\n");

    historySizes.push_back(0);
    for (unsigned i=2; i <= max_history_length; i <<= 1)
        historySizes.push_back(i);

    maxBranches = 0;
    selectedTargetBits = 5;
    selectedTargetMask = (1 << selectedTargetBits) - 1;

    unsigned set_bits = log((double)num_rows, 2);

    num_tables = historySizes.size();
    paths = std::vector<SimplBlockCache>();
    paths.reserve(num_tables);

    for (unsigned i = 0; i < num_tables; ++i) {
        paths[i].init(counter_bits, set_bits, tag_bits, associativity);
    }

}

std::ptrdiff_t PHAST::checkInst(DynInstPtr load, BranchHistory branchHistory) {
    std::ptrdiff_t distance = -1;

    auto begin = branchHistory.begin();
    InstSeqNum branch_seq_num = load->seqNum;
    while (begin != branchHistory.end() && load->seqNum > branch_seq_num) {
        branch_seq_num = begin->seqNum;
        begin++;
    }

    uint64_t hash;
    std::ptrdiff_t tmp_distance;
    for (unsigned i = 0; i <= maxBranches && i < historySizes.size(); i++) {
        hash = generateBranchHash(historySizes[i], i, begin);
        tmp_distance = paths[i].predict(load->pcState()->instAddr(), hash);
        if (tmp_distance != -1) {
            distance = tmp_distance;
            branch_history_length = i;
            load->memDepInfo.predBranchHistLength = i;
            load->memDepInfo.predictorHash = hash;
        }
    }

    return distance;
}

void PHAST::violation(InstSeqNum store_seq_num, DynInstPtr load, BranchHistory branchHistory) {
    uint64_t first_jump = 0;
    uint64_t path_hash = 0;
    unsigned hist_items = 0;
    bitset<BITSETSIZE> h(0);
    int bits = 60;
    int hs_idx = 0;

    //corner case of a violation before any branches
    //FIXME: original code sets num branches to 0 if there are no inflight branches, so
    //do we have to track which committed branches aren't too old??
    if (branchHistory.empty()) return;

    //taking branch history from commit so first branch is always older than the load
    auto br_it = branchHistory.begin();
    InstSeqNum branch_seq_num;
    do {
        branch_seq_num = br_it->seqNum;
        br_it++;
    } while (br_it != branchHistory.end() && store_seq_num > branch_seq_num);

    unsigned num_branches = (unsigned)std::distance(branchHistory.begin(), br_it);

    //quantise num branches to first lowest path size
    unsigned path_index = 0;
    for (unsigned i = historySizes.size(); i-- > 0;) {
        unsigned size = historySizes[i];
        if ((num_branches & size) == size) {
            path_index = size;
            break;
        }
    }

    path_hash = generateBranchHash(num_branches, path_index, branchHistory.begin());

    paths[path_index].update(load->pcState().instAddr(), path_hash, load->memDepInfo->storeQueueDistance);
    maxBranches = std::max(maxBranches, path_index);
}

void commit(DynInstPtr inst) {

    if (inst->isStore()) return;

    if (!inst->memDepInfo.predStoreAddr) return;

    bool misprediction;
    Addr ld_s = inst->effAddr;
    Addr ld_e = ld_s + inst->effSize;
    Addr st_s = inst->memDepInfo.predStoreAddr;
    Addr st_e = st_s + inst->memDepInfo.predStoreSize;
    bool store_has_lower_limit = ld_s >= st_s;
    bool store_has_upper_limit = ld_e <= st_e;
    bool lower_load_has_store_part = ld_s < st_e;
    bool upper_load_has_store_part = ld_e > st_s;

    if ((store_has_lower_limit && store_has_upper_limit) ||
        (store_has_lower_limit && lower_load_has_store_part) ||
        (store_has_upper_limit && upper_load_has_store_part) ||
        (lower_load_has_store_part && upper_load_has_store_part))
        misprediction = false;
    else
        misprediction = true;

    int branch_history = inst->memDepInfo.predBranchHistLength;

    paths[branch_history].updateCommit(inst->pcState()->instAddr(),
                                       inst->memDepInfo.predictorHash,
                                       misprediction);

}

uint64_t PHAST::generateBranchHash(unsigned num_branches, unsigned path_index, BranchHistory::iterator branchHistory) {
    deque<uint64_t> tmp_path;
    tmp_path.clear();
    int bits = 60;
    bitset<BITSETSIZE> h = branchHistory[num_branches].target & selectedTargetMask;  // This is the +1 branch.
    tmp_path.push_back(branchHistory[num_branches].target);

    unsigned hist_items = 0;
    for (auto pos = branchHistory; pos != branchHistory.end() && hist_items < num_branches; pos++) {
        if (!branchHistory[pos].indirect) {
            h <<= 1;
            h[0] = branchHistory[pos].taken;
            ++hist_items;
            ++bits;
            tmp_path.push_back(branchHistory[pos].taken);
        } else if (selectedTargetMask != 0) {
            h <<= selectedTargetBits;
            h ^= (branchHistory[pos].target & selectedTargetMask);
            ++hist_items;
            bits += selectedTargetBits;
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

void PHAST::clear() {
   maxBranches = 0;

    for (unsigned i = 0; i < paths.size(); ++i) {
        paths[i].clear();
    }

}

int PHAST::SimplBlockCache::init(unsigned counter_bits, unsigned set_bits, unsigned tag_bits, unsigned associativity) {
    tagBits = tag_bits;
    setBits = set_bits;
    associativity = associativity;
    maxCounterValue = (1 << counter_bits) - 1;
    lruCounter = 0;

    cache = std::vector<std::vector<Entry>>(1 << setBits);

    for (uint64_t i = 0; i < (1ULL << setBits); i++) {
        cache[i] = vector<Entry>(associativity);

        for (uint32_t j = 0; j < associativity; j++) {
            cache[i][j].tag = 0;
            cache[i][j].distance = 0;
            cache[i][j].lru = 0;
            cache[i][j].counter = 0;
        }
    }
    return (1 << setBits) * associativity;
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
    uint64_t index = xorFold(0, (ldPC ^ history), setBits);
    return index;
}
uint64_t PHAST::SimplBlockCache::getTag(Addr pc, uint64_t history) const {
    uint64_t ldPC = pc.getAddress();
    ldPC = (ldPC ^ (ldPC >> 3) ^ (ldPC >> 7));
    uint64_t tag = xorFold(0, (ldPC ^ history), tagBits);
    return tag;
}

Entry* PHAST::SimplBlockCache::findEntry(Addr pc, uint64_t history) {
    uint64_t set = getIndex(pc, history);
    uint64_t tag = getTag(pc, history);
    for (uint32_t i = 0; i < associativity; i++) {
        if (cache[set][i].tag == tag) {
            return &(cache[set][i]);
        }
    }
    return nullptr;
}

Entry* PHAST::SimplBlockCache::getLRUEntry(uint64_t set) {
    uint32_t lru_way = 0;
    uint64_t lru_value = cache[set][lru_way].lru;
    for (uint32_t i = 0; i < associativity; i++) {
        if (cache[set][i].lru < lru_value) {
            lru_way = i;
            lru_value = cache[set][lru_way].lru;
        }
    }
    return &(cache[set][lru_way]);
}

void PHAST::SimplBlockCache::updateLRU(Entry* entry) {
    entry->lru = lru_counter;
    lru_counter++;
}

std::ptrdiff_t PHAST::SimplBlockCache::predict(Addr pc, uint64_t history) {
    auto entry = findEntry(pc, history);
    if (entry == nullptr || entry->counter == 0 || entry->distance == 0) { // no prediction for this PC
        return 0;
    }

    updateLRU(entry);

    return entry->distance;
}

void PHAST::SimplBlockCache::update(Addr pc, uint64_t history, std::ptrdiff_t distance) {
    auto entry = findEntry(pc, history);
    if (entry == nullptr) {
        // no prediction for this entry so far, so allocate one
        entry = getLRUEntry(getIndex(pc, history));
        entry->tag = getTag(pc, history);
        entry->distance = distance;
        entry->counter = maxCountervalue;
        updateLRU(entry);
    } else {
        entry->distance = distance;
        entry->counter = maxCountervalue;
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

void PHAST::SimplBlockCache::clear() {

    for (uint64_t i = 0; i < (1ULL << setBits); i++) {
        for (uint32_t j = 0; j < associativity; j++) {
            cache[i][j].tag = 0;
            cache[i][j].distance = 0;
            cache[i][j].lru = 0;
            cache[i][j].counter = 0;
        }
    }

}

} // namespace o3
} // namespace gem5
