#include <cstdint>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct StoreInfo {
    uint64_t pc;
    uint64_t seq;
};

// Hash for pair<uint64_t, uint64_t>
struct PairHash {
    size_t operator()(const pair<uint64_t, uint64_t>& p) const {
        return hash<uint64_t>()(p.first) ^ (hash<uint64_t>()(p.second) << 1);
    }
};

// Combine addr + size into a single key
static inline uint64_t make_key(uint64_t addr, uint32_t size) {
    return (addr << 8) | size;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <memtrace_file> <predictions_file>\n";
        return 1;
    }

    ifstream infile(argv[1]);
    ofstream outfile(argv[2]);
    if (!infile) {
        cerr << "Error opening memtrace file\n";
        return 1;
    }

    if (!outfile) {
        cerr << "Error opening predictions file\n";
        return 1;
    }

    unordered_map<uint64_t, StoreInfo> last_store;   // key -> (pc, seq)
    unordered_map<uint64_t, uint64_t> last_store_by_pc; // pc -> seq

    // Deduplication
    unordered_set<pair<uint64_t, uint64_t>, PairHash> seen;

    // Load-PC consistency enforcement
    unordered_map<uint64_t, uint64_t> load_to_store; // load_pc -> store_pc
    unordered_set<uint64_t> invalid_loads;           // load PCs that violated rule

    string line;

    while (getline(infile, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        stringstream ss(line);

        uint64_t seq;
        uint64_t pc, addr;
        uint32_t size;
        char op;

        if (!(ss >> seq >> hex >> pc >> hex >> addr >> size >> op)) continue;

        uint64_t key = make_key(addr, size);

        if (op == 'S') {
            // Update last store for this address
            last_store[key] = {pc, seq};

            // Update last occurrence of this PC
            last_store_by_pc[pc] = seq;
        }
        else if (op == 'L') {
            auto it = last_store.find(key);
            if (it == last_store.end())
                continue;

            uint64_t store_pc  = it->second.pc;
            uint64_t store_seq = it->second.seq;

            // Validate store is still current for its PC
            auto pc_it = last_store_by_pc.find(store_pc);
            if (pc_it == last_store_by_pc.end() ||
                pc_it->second != store_seq)
                continue;

            // Enforce one-store-per-load-PC constraint
            if (invalid_loads.count(pc))
                continue;

            auto lt = load_to_store.find(pc);

            if (lt == load_to_store.end()) {
                // First observation
                load_to_store[pc] = store_pc;
            } else if (lt->second != store_pc) {
                // Conflict → invalidate this load PC permanently
                invalid_loads.insert(pc);
                load_to_store.erase(pc);
                continue;
            }

            // Emit only once per pair
            pair<uint64_t, uint64_t> dep = {pc, store_pc};
            if (seen.insert(dep).second) {
                outfile << hex << pc << " " << store_pc << "\n";
            }
        }
    }

    outfile.flush();
    outfile.close();
    infile.close();
    return 0;
}