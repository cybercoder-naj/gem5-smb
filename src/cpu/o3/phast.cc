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
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR#include "mem_dep_unit.hh"
 TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cpu/o3/phast.hh"
#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "base/compiler.hh"
#include "base/debug.hh"
#include "cpu/o3/dyn_inst.hh"
#include "cpu/o3/inst_queue.hh"
#include "cpu/o3/limits.hh"
#include "dyn_inst_ptr.hh"

#include <cmath>

namespace gem5
{

namespace o3
{

PHAST::PHAST(const BaseO3CPUParams &params, MemDepUnit *mem_dep_unit) {

    assert(isPowerOf2(params.num_rows) && "Invalid number of rows per table!\n");

    //TODO: paramertise this with a string and parse it into a list
    historySizes.assign({0, 2, 4, 6, 8, 12, 16, 32});

    unsigned set_bits = (unsigned)log2((double)(params.num_rows));

    maxBranches = 0;
    selectedTargetBits = 5;
    selectedTargetMask = (1 << selectedTargetBits) - 1;
    memDepUnit = mem_dep_unit;

    unsigned num_tables = historySizes.size();
    paths = std::vector<SimplBlockCache>();
    paths.resize(num_tables, SimplBlockCache());

    for (unsigned i = 0; i < num_tables; ++i) {
        paths[i].init((uint32_t)(params.set_bits), (uint32_t)(params.associativity), (uint32_t)(params.tag_bits), (uint32_t)(params.max_counter_value));
    }

}

PHAST::~PHAST()
{
}

void PHAST::init(const BaseO3CPUParams &params, MemDepUnit *mem_dep_unit) {

    assert(isPowerOf2(params.num_rows) && "Invalid number of rows per table!\n");

    //TODO: paramertise this with a string and parse it into a list
    //if you still want power estimation also need to have a non fucked version
    //of my stats counters though, sorry!
    historySizes.assign({0, 2, 4, 6, 8, 12, 16, 32});

    maxBranches = 0;
    selectedTargetBits = 5;
    selectedTargetMask = (1 << selectedTargetBits) - 1;
    memDepUnit = mem_dep_unit;

    unsigned set_bits = (unsigned)log2((double)(params.num_rows));

    debug = true;

    unsigned num_tables = historySizes.size();
    paths = std::vector<SimplBlockCache>();
    paths.resize(num_tables, SimplBlockCache());

    for (unsigned i = 0; i < num_tables; ++i) {
        paths[i].init((uint32_t)(params.set_bits), (uint32_t)(params.associativity), (uint32_t)(params.tag_bits), (uint32_t)(params.max_counter_value));
    }

}

void PHAST::insertStore(Addr store_PC, InstSeqNum store_seq_num, ThreadID id) {
    storeMap[store_PC] = store_seq_num;
}

PredictionResult PHAST::checkInst(Addr load_pc, InstSeqNum load_seq_num, BranchHistory branchHistory) {

    struct PredictionResult prediction;
    prediction.seqNum = 0;

    if (branchHistory.size() == 0) return prediction;
    auto begin = branchHistory.begin();
    while (begin != branchHistory.end() && begin->seqNum > load_seq_num) {
        begin++;
    }
    if (begin == branchHistory.end()) return prediction; //no +1 branch

    if (historySizes[maxBranches] > branchHistory.size()) {
        int i;
        for (i=0; historySizes[i] <= branchHistory.size(); i++);
        maxBranches = i-1;
    }

    uint64_t hash;
    Addr store_pc;
    for (unsigned i = 0; i <= maxBranches && i < historySizes.size(); i++) {
        hash = generateBranchHash(i, historySizes[i], begin, branchHistory.end());
        store_pc = paths[i].predict(load_pc, hash);
        if (store_pc) {
            auto tmp_seq_num_it = storeMap.find(store_pc);
            if (tmp_seq_num_it == storeMap.end()) continue;
            // all paths are read on prediction, so just use that stat to calc reads
            ++(*(memDepUnit->pathWrites[i]));
            prediction.seqNum = tmp_seq_num_it->second;
            prediction.predBranchHistLength = i;
            prediction.predictorHash = hash;
        }
    }

    return prediction;
}

void PHAST::violation(Addr load_pc, InstSeqNum store_seq_num, Addr store_pc, std::ptrdiff_t storeQueueDistance,
                      BranchHistory branchHistory) {

    //corner case of a violation before any branches or no +1 branch
    if (branchHistory.empty() || branchHistory.back().seqNum > store_seq_num) return;

    //taking branch history from commit so first branch is always older than the load
    auto br_it = branchHistory.begin();
    InstSeqNum branch_seq_num;
    do {
        branch_seq_num = br_it->seqNum;
        br_it++;
    } while (br_it != branchHistory.end() && branch_seq_num > store_seq_num);

    unsigned num_branches = (unsigned)std::distance(branchHistory.begin(), br_it);

    //quantise num branches to first lowest path size
    unsigned i;
    for (i=1; i < historySizes.size(); i++) {
        unsigned size = historySizes[i];
        if (num_branches < size) {
            num_branches = historySizes[i-1];
            break;
        }
    }

    uint64_t path_hash = generateBranchHash(i, num_branches, branchHistory.begin(), branchHistory.end());
    paths[i].update(load_pc, path_hash, store_pc);

    if (store_pc == 4204896 && path_hash == 0) {
        std::cout << "Store found\n";
        std::cout << "hash: " << path_hash << "\n";
        std::cout << "num branches (quant): " << i << "\n";
        std::cout << "Branch history:\n";
        for (int n=0; n < i; n++) {
            std::cout << "address: " << branchHistory[n].pc << "\n";
            std::cout << "taken: " << branchHistory[n].taken << "\n";
            std::cout << "indirect: " << branchHistory[n].indirect << "\n";
            std::cout << "target: " << branchHistory[n].target << "\n";
        }   
        std::cout << "\n";
    }

    maxBranches = std::max(maxBranches, i);
    ++(*(memDepUnit->pathReads[i]));
    ++(*(memDepUnit->pathWrites[i]));

}

void PHAST::commit(Addr load_pc, Addr load_addr, unsigned load_size, Addr store_addr, unsigned store_size, unsigned path_index, uint64_t predictor_hash) {

    //TODO: in real hardware, would it still have to perform a lookup?
    //i.e., should we still increment a counter for power estimation purposes
    if (!predictor_hash) return;

    bool misprediction;
    Addr ld_s = load_addr;
    Addr ld_e = ld_s + load_size;
    Addr st_s = store_addr;
    Addr st_e = st_s + store_size;
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

    if (misprediction) ++(memDepUnit->stats.PHASTMispredictions);
    else ++(memDepUnit->stats.PHASTCorrectPredictions);

    paths[path_index].updateCommit(load_pc, predictor_hash, misprediction);
    ++(*(memDepUnit->pathReads[path_index]));
    ++(*(memDepUnit->pathWrites[path_index]));

}

uint64_t PHAST::generateBranchHash(unsigned path_index, unsigned num_branches, BranchHistory::iterator branchHistoryBegin, BranchHistory::iterator branchHistoryEnd) {
    std::deque<uint64_t> tmp_path;
    tmp_path.clear();
    int bits = 60;
    bitset<BITSETSIZE> h = branchHistoryBegin[num_branches].target & selectedTargetMask;  // This is the +1 branch.
    tmp_path.push_back(branchHistoryBegin[num_branches].target);

    unsigned hist_items = 0;
    for (auto br_it = branchHistoryBegin; br_it != branchHistoryEnd && hist_items < num_branches; br_it++) {
        if (!br_it->indirect) {
            h <<= 1;
            h[0] = br_it->taken;
            ++hist_items;
            ++bits;
            tmp_path.push_back(br_it->taken);
        } else if (selectedTargetMask != 0) {
            h <<= selectedTargetBits;
            h ^= (br_it->target & selectedTargetMask);
            ++hist_items;
            bits += selectedTargetBits;
            tmp_path.push_back(br_it->target);
        }
    }

    return foldHistory(h, bits, paths[path_index].getSetBits(), paths[path_index].getTagBits());
}

uint64_t PHAST::foldHistory(bitset<BITSETSIZE> h, int bits, unsigned _setBits, unsigned _tagBits) {
    int width = _setBits + _tagBits;
    bitset<BITSETSIZE> mask((1ULL << width) - 1);
    uint64_t hash = 0;

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

int PHAST::SimplBlockCache::init(uint32_t set_bits, uint32_t _associativity, uint32_t tag_bits, uint32_t max_counter_value) {

    tagBits = tag_bits;
    setBits = set_bits;
    associativity = _associativity;
    maxCounterValue = max_counter_value;
    lruCounter = 0;

    cache = std::vector<std::vector<Entry>>();
    cache.resize((1 << setBits), std::vector<Entry>());

    for (uint64_t i = 0; i < (1ULL << setBits); i++) {
        cache[i].resize(associativity, {0,0,0,0});
    }

    //num entries for this path
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
    pc = (pc ^ (pc >> 2) ^ (pc >> 5));
    uint64_t index = xorFold(0, (pc ^ history), setBits);
    return index;
}
uint64_t PHAST::SimplBlockCache::getTag(Addr pc, uint64_t history) const {
    pc = (pc ^ (pc >> 3) ^ (pc >> 7));
    uint64_t tag = xorFold(0, (pc ^ history), tagBits);
    return tag;
}

PHAST::SimplBlockCache::Entry *PHAST::SimplBlockCache::findEntry(Addr pc, uint64_t history) {
    uint64_t set = getIndex(pc, history);
    uint64_t tag = getTag(pc, history);
    for (uint32_t i = 0; i < associativity; i++) {
        if (cache[set][i].tag == tag) {
            return &(cache[set][i]);
        }
    }
    return nullptr;
}

PHAST::SimplBlockCache::Entry *PHAST::SimplBlockCache::getLRUEntry(uint64_t set) {
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
    entry->lru = lruCounter;
    lruCounter++;
}

Addr PHAST::SimplBlockCache::predict(Addr pc, uint64_t history) {
    auto entry = findEntry(pc, history);
    if (entry == nullptr || entry->counter == 0 || entry->store_pc == 0) { // no prediction for this PC
        return 0;
    }

    updateLRU(entry);

    return entry->store_pc;
}

void PHAST::SimplBlockCache::update(Addr pc, uint64_t history, Addr store_pc) {
    auto entry = findEntry(pc, history);
    if (entry == nullptr) {
        // no prediction for this entry so far, so allocate one
        entry = getLRUEntry(getIndex(pc, history));
        entry->tag = getTag(pc, history);
        entry->store_pc = store_pc;
        entry->counter = maxCounterValue;
        updateLRU(entry);
    } else {
        entry->store_pc = store_pc;
        entry->counter = maxCounterValue;
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
        entry->counter = maxCounterValue;
    }

    updateLRU(entry);
}

void PHAST::SimplBlockCache::clear() {

    for (uint64_t i = 0; i < (1ULL << setBits); i++) {
        for (uint32_t j = 0; j < associativity; j++) {
            cache[i][j].tag = 0;
            cache[i][j].store_pc = 0;
            cache[i][j].lru = 0;
            cache[i][j].counter = 0;
        }
    }

}

} // namespace o3
} // namespace gem5
