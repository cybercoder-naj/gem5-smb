#include <cstdint>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

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

    // Address to last PC writer
    unordered_map<uint64_t, uint64_t> memory {};
    unordered_set<uint64_t> invalid_loads {};

    // LoadPC -> StorePC
    unordered_map<uint64_t, uint64_t> deps {};

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

        if (op == 'S') {
            for (auto addr_byte = addr; addr_byte < addr + size; ++addr_byte)
                memory[addr_byte] = pc;
        } else if (op == 'L') {
            bool valid_dep = true;
            optional<uint64_t> store_pc = nullopt;

            for (auto addr_byte = addr; addr_byte < addr + size; ++addr_byte) {
                auto mem_it = memory.find(addr_byte);
                if (mem_it == memory.end()) {
                    valid_dep = false;
                    break;
                }
                if (!store_pc.has_value()) {
                    store_pc = mem_it->second;
                } else if (store_pc.value() != memory[addr_byte]) {
                    valid_dep = false;
                    break;
                }
            }

            if (store_pc.has_value() && valid_dep) {
                if (deps.count(pc) && deps[pc] != store_pc)
                    invalid_loads.insert(pc);
                else
                    deps[pc] = store_pc.value();
            }
        } else {
            cerr << "Unrecognised character: " << op << std::endl;
            exit(1);
        }
    }

    for (const auto& [load_pc, store_pc] : deps) {
        if (!invalid_loads.count(load_pc))
            outfile << hex << load_pc << " " << hex << store_pc << "\n";
    }

    outfile.flush();
    outfile.close();
    infile.close();
    return 0;
}