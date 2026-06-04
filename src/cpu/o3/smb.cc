/*
 * Copyright ??
 */

#include "cpu/o3/smb.hh"

#include <algorithm>
#include <fstream>
#include <sstream>

#include "base/trace.hh"
#include "debug/SMB.hh"

namespace gem5
{

namespace o3
{

SMB::SMB(const std::string &_my_name) :
    _name(_my_name),
    storeAddrToSeqNum(),
    loadSeqNumToAddr()
{
  const char* env = std::getenv("SMB_PREDICTIONS_FILE");
  if (!env) {
    DPRINTF(SMB, "SMB_PREDICTIONS_FILE environment variable not set. No predictions loaded.\n");
    return;
  }

  std::ifstream infile;
  infile.open(env);
  if (!infile.is_open()) {
    DPRINTF(SMB, "Could not open SMB predictions file\n");
  }

  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;

    std::stringstream ss(line);
    Addr l_pc, s_pc;
    if (!(ss >> std::hex >> l_pc >> std::hex >> s_pc)) continue;

    predictions[l_pc] = s_pc;
  }
  infile.close();
}

InstSeqNum
SMB::predictSourceStore(InstSeqNum load_seq_num)
{
  Addr load_pc = loadSeqNumToAddr[load_seq_num];
  if (load_pc == 0) {
    return 0;
  }

  if (predictions.count(load_pc)) {
    InstSeqNum store_seq_num = storeAddrToSeqNum[predictions[load_pc]];
    return store_seq_num;
  }
  
  return 0;
}

void
SMB::squash(InstSeqNum squashed_inst_seq)
{
  // Remove any predictions that are associated with loads that have sequence numbers less than or equal to the committed sequence number.
  for (auto it = loadSeqNumToAddr.begin(); it != loadSeqNumToAddr.end(); ) {
    if (it->first > squashed_inst_seq) {
      it = loadSeqNumToAddr.erase(it);
    } else {
      ++it;
    }
  }

  for (auto it = storeAddrToSeqNum.begin(); it != storeAddrToSeqNum.end(); ) {
    if (it->second > squashed_inst_seq) {
      it = storeAddrToSeqNum.erase(it);
    } else {
      ++it;
    }
  }
}

void 
SMB::removeUpTo(InstSeqNum seq_num)
{
  // Remove any predictions that are associated with loads that have sequence numbers less than or equal to the committed sequence number.
  for (auto it = loadSeqNumToAddr.begin(); it != loadSeqNumToAddr.end(); ) {
    if (it->first <= seq_num) {
      it = loadSeqNumToAddr.erase(it);
    } else {
      ++it;
    }
  }

  for (auto it = storeAddrToSeqNum.begin(); it != storeAddrToSeqNum.end(); ) {
    if (it->second <= seq_num) {
      it = storeAddrToSeqNum.erase(it);
    } else {
      ++it;
    }
  }
}

bool
SMB::deletePrediction(Addr load_addr)
{
  auto it = predictions.find(load_addr);
  if (it != predictions.end()) {
    predictions.erase(it);
    return true;
  }
  return false;
}

} // namespace o3
} // namespace gem5
