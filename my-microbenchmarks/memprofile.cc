#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

typedef uint64_t InstSeqNum;
typedef uint64_t Addr;

struct MemAccessInfo {
    Addr pc;
    Addr eff_addr;
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

    std::unordered_map<Addr, MemAccessInfo> lastWriter;

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
            if (lastWriter.find(eff_addr) != lastWriter.end()) {
                MemAccessInfo info = lastWriter[eff_addr];
                std::cout << "Load at PC: " << std::hex << inst_addr
                          << " reads from store at PC: " << std::hex << info.pc
                          << " with effective address: " << std::hex << eff_addr
                          << "\n";
                outfile << inst_addr << " " << info.pc << " " << "\n";
            } else {
                std::cout << "Load at PC: " << std::hex << inst_addr
                          << " has no prior store for effective address: "
                          << std::hex << eff_addr << "\n";
            }
            break;
          case 'S':
            lastWriter[eff_addr] = {inst_addr, eff_addr};
            break;
        }
      } else {
        std::cerr << "Invalid line format: " << line << std::endl;
        return 1;
      } 
    }

    infile.close();
    return 0;
}