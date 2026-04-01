/*
 * Copyright ??
 */

#include "cpu/o3/smb.hh"

#include <fstream>
#include <sstream>

namespace gem5
{

namespace o3
{

SMB::SMB(const std::string &_my_name) :
    _name(_my_name)
{}

InstSeqNum
SMB::predictSourceStore(InstSeqNum load_seq_num)
{
    if (predictions.empty()) {
      loadPredictions();
    }

    if (predictions.find(load_seq_num) == predictions.end()) {
        return -1;
    }
    return predictions[load_seq_num];
}

void
SMB::loadPredictions()
{
  std::ifstream infile("/home/nj421/gem5-phast/my-progs/exit.pred");
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