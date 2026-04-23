#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

typedef uint64_t InstSeqNum;
typedef uint64_t Addr;

struct MemKey {
    Addr eff_addr;
    unsigned int eff_size;

    bool operator==(const MemKey& other) const {
        return eff_addr == other.eff_addr && eff_size == other.eff_size;
    }
};

struct MemKeyHash {
    std::size_t operator()(const MemKey& k) const {
        return std::hash<Addr>()(k.eff_addr) ^ std::hash<unsigned int>()(k.eff_size);
    }
};


int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <memtrace_file> <output_file>" << std::endl;
        return 1;
    }

    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    std::ofstream outfile(argv[2]);
    if (!outfile.is_open()) {
        std::cerr << "Could not open file: " << argv[2] << std::endl;
        return 1;
    }

    // (addr, size) -> store_pc
    std::unordered_map<MemKey, Addr, MemKeyHash> writers{};
    // load_pc -> (store_pc, consistent)
    std::unordered_map<Addr, std::pair<Addr, bool>> readers{};

    std::string line;
    while (std::getline(infile, line)) {
      if (line.empty() || line[0] == '#') {
          continue; // Skip empty lines and comments
      }

      std::stringstream ss(line);
      InstSeqNum seq_num; 
      Addr inst_addr, eff_addr;
      unsigned int eff_size;
      char load_store;

      if (ss >> seq_num >> std::hex >> inst_addr >> std::hex >> eff_addr >> std::dec >> eff_size >> load_store) {
        MemKey key{eff_addr, eff_size};

        switch (load_store) {
            case 'S':
                writers[key] = inst_addr;
                break;

            case 'L':
                if (!writers.count(key)) {
                    break;
                } 
                Addr store_pc = writers[key];

                if (!readers.count(inst_addr)) {
                    readers[inst_addr] = {store_pc, true};
                } else if (readers[inst_addr].first != store_pc) {
                    readers[inst_addr].second = false; // Mark as inconsistent
                }
        }
      } else {
        std::cerr << "Invalid line format: " << line << std::endl;
        return 1;
      } 
    }

    for (const auto& it : readers) {
        if (it.second.second) { // if is consistent
            // print load_pc and store_pc in hex
            outfile << std::hex << it.first << " " << std::hex << it.second.first << "\n";
        }
    }

    infile.close();
    return 0;
}