#include <array>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

constexpr uint64_t SQSize = 114;

struct MemBody {
    uint64_t storePc;
    uint64_t baseAddr;
    uint64_t instSeqNum;
};

struct SQEntry {
    uint64_t instSeqNum;
    bool isStore;
    bool isBypassable;
};

int main(int argc, char* argv[]) {
    using namespace std;

    if (argc != 3) {
        cerr << "Usage: " << argv[0]
             << " <memtrace_file> <predictions_file>\n";
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

    // =========================================================================
    // Memory state
    // =========================================================================

    // byte-address -> producer store
    unordered_map<uint64_t, MemBody> memory;
    memory.reserve(1 << 20);

    // ring-buffer SQ
    array<SQEntry, SQSize> storeQueue {};

    // instSeqNum -> logical SQ position
    unordered_map<uint64_t, uint64_t> seqToPos;
    seqToPos.reserve(256);

    uint64_t tail = 0;

    // =========================================================================
    // Main loop
    // =========================================================================

    string line;

    while (getline(infile, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        uint64_t seq;
        uint64_t pc;
        uint64_t addr;
        uint32_t size;
        int bypassable;
        char op;

        if (sscanf(line.c_str(),
                   "%" SCNu64 " %" SCNx64 " %" SCNx64 " %u %d %c",
                   &seq,
                   &pc,
                   &addr,
                   &size,
                   &bypassable,
                   &op) != 6) {
            continue;
        }

        // =====================================================================
        // Allocate / Atomic
        // =====================================================================

        if (op == 'A') {
            uint64_t slot = tail % SQSize;

            // remove overwritten entry
            seqToPos.erase(storeQueue[slot].instSeqNum);

            storeQueue[slot] = {
                seq,
                false,
                false
            };

            seqToPos[seq] = tail;
            ++tail;

            continue;
        }

        // =====================================================================
        // Store
        // =====================================================================

        if (op == 'S') {
            uint64_t slot = tail % SQSize;

            // remove overwritten entry
            seqToPos.erase(storeQueue[slot].instSeqNum);

            storeQueue[slot] = {
                seq,
                true,
                bypassable == 1
            };

            seqToPos[seq] = tail;
            ++tail;

            MemBody body {
                pc,
                addr,
                seq
            };

            // byte-granularity tracking
            for (uint64_t a = addr; a < addr + size; ++a) {
                memory[a] = body;
            }

            continue;
        }

        // =====================================================================
        // Load
        // =====================================================================

        if (op == 'L') {
            bool validDep = true;
            MemBody* storeInfo = nullptr;

            for (uint64_t a = addr; a < addr + size; ++a) {
                auto memIt = memory.find(a);

                if (memIt == memory.end()) {
                    validDep = false;
                    break;
                }

                MemBody& mem = memIt->second;

                // ensure full forwarding from same store
                if (mem.baseAddr != addr) {
                    validDep = false;
                    break;
                }

                if (!storeInfo) {
                    storeInfo = &mem;
                } else if (storeInfo->storePc != mem.storePc) {
                    validDep = false;
                    break;
                }
            }

            if (!validDep || !storeInfo) {
                outfile << hex << pc << " 0\n";
                continue;
            }

            // ================================================================
            // SQ lookup
            // ================================================================

            auto posIt = seqToPos.find(storeInfo->instSeqNum);

            if (posIt == seqToPos.end()) {
                outfile << hex << pc << " 0\n";
                continue;
            }

            uint64_t pos = posIt->second;

            SQEntry& entry = storeQueue[pos % SQSize];

            // overwritten ring-buffer protection
            if (entry.instSeqNum != storeInfo->instSeqNum) {
                outfile << hex << pc << " 0\n";
                continue;
            }

            if (!entry.isStore || !entry.isBypassable) {
                outfile << hex << pc << " 0\n";
                continue;
            }

            uint64_t distance = tail - pos;

            outfile << hex << pc
                    << " "
                    << dec << distance
                    << "\n";

            continue;
        }

        cerr << "Unrecognized op: " << op << "\n";
        return 1;
    }

    outfile.flush();

    return 0;
}
