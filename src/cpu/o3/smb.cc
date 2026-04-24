/*
 * Copyright ??
 */

#include "cpu/o3/smb.hh"

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
{
  const char* env = std::getenv("SMB_PREDICTIONS_FILE");
  if (!env) {
    DPRINTF(SMB, "SMB_PREDICTIONS_FILE environment variable not set. No predictions loaded.\n");
    return;
  }

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
  
  // std::string line;
  // while (std::getline(infile, line)) {
  //   if (line.empty()) continue;

  //   std::stringstream ss(line);
  //   Addr l_pc, s_pc;
  //   if (!(ss >> std::hex >> l_pc >> std::hex >> s_pc)) continue;

  //   if (l_pc == load_pc) {
  //     InstSeqNum store_seq_num = storeAddrToSeqNum[s_pc];
  //     assert(store_seq_num != 0);
  //     return store_seq_num;
  //   }

  //   predictions[l_pc] = s_pc;
  // }


  return 0;
}

void
SMB::squash(InstSeqNum squashed_seq_num)
{
  // Remove any predictions that are associated with loads that have sequence numbers greater than the squashed sequence number.
  for (auto it = loadSeqNumToAddr.begin(); it != loadSeqNumToAddr.end(); ) {
    if (it->first > squashed_seq_num) {
      it = loadSeqNumToAddr.erase(it);
    } else {
      ++it;
    }
  }

  for (auto it = storeAddrToSeqNum.begin(); it != storeAddrToSeqNum.end(); ) {
    if (it->second > squashed_seq_num) {
      it = storeAddrToSeqNum.erase(it);
    } else {
      ++it;
    }
  }
}

} // namespace o3
} // namespace gem5
