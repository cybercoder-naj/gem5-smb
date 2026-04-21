#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

typedef uint64_t InstSeqNum;
typedef uint64_t Addr;

struct MemAccessInfo {
    Addr s_pc;
    Addr l_pc;
    Addr eff_addr;
    bool print;
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

    std::unordered_map<Addr, MemAccessInfo> writers;

    std::string line;
    while (std::getline(infile, line)) {
      if (line.empty() || line[0] == '#') {
          continue; // Skip empty lines and comments
      }

      std::stringstream ss(line);
      InstSeqNum seq_num; 
      Addr inst_addr, eff_addr;
      char load_store;

      if (ss >> seq_num >> std::hex >> inst_addr >> std::hex >> eff_addr >> load_store) {
        switch (load_store) {
          case 'L':
            if (writers.find(eff_addr) != writers.end()) {
                MemAccessInfo info = writers[eff_addr];
                std::cout << "Load at PC: " << std::hex << inst_addr
                          << " reads from store at PC: " << std::hex << info.s_pc
                          << " with effective address: " << std::hex << eff_addr
                          << "\n";
                writers[eff_addr].l_pc = inst_addr;
            } else {
                std::cout << "Load at PC: " << std::hex << inst_addr
                          << " has no prior store for effective address: "
                          << std::hex << eff_addr << "\n";
            }
            break;
          case 'S':
            if (writers.find(eff_addr) != writers.end()) {
                if (inst_addr != writers[eff_addr].s_pc) {
                    writers[eff_addr].print = false;
                }
            } else {
                writers[eff_addr] = {inst_addr, 0, eff_addr, true};
            }
            break;
        }
      } else {
        std::cerr << "Invalid line format: " << line << std::endl;
        return 1;
      } 
    }

    for (const auto& entry : writers) {
        if (entry.second.print) {
            outfile << std::hex << entry.second.l_pc << " " << std::hex << entry.second.s_pc << "\n";
        }
    }

    infile.close();
    return 0;
}