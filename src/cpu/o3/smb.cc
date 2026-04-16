/*
 * Copyright ??
 */

#include "cpu/o3/smb.hh"

#include <cassert>
#include <fstream>
#include <sstream>

#include "base/trace.hh"
#include "debug/SMB.hh"

namespace gem5
{

namespace o3
{

SMB::SMB(const std::string &_my_name) :
    _name(_my_name)
{}

void 
SMB::registerStore(InstSeqNum store) {
    DPRINTF(SMB, "Registering store [sn:%llu]\n", store);

    sourceStores.push(store);
}

InstSeqNum
SMB::predictSourceStore()
{
  if (sourceStores.empty()) {
    DPRINTF(SMB, "No source store predictions available.\n");
    return 0; 
  }

  auto seq_num = sourceStores.top();
  DPRINTF(SMB, "Predicting source store [sn:%llu]\n", seq_num);

  sourceStores.pop();
  return seq_num;
}

void
SMB::loadPredictions()
{
  std::ifstream infile("/workspaces/gem5-smb/my-progs/exit.pred");
  if (!infile.is_open()) {
    throw std::runtime_error("Could not open SMB predictions file.");
  }

  std::string line;
  while (std::getline(infile, line)) {
    std::stringstream ss(line);
    int load_seq_num, store_seq_num;
    char comma;

    if (ss >> load_seq_num >> comma >> store_seq_num) {
      predictions[load_seq_num] = store_seq_num;
    } else {
      throw std::runtime_error("Invalid line in SMB predictions file: " + line);
    }
  }
}

} // namespace o3
} // namespace gem5
