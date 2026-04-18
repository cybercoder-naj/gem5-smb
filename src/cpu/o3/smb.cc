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
}

InstSeqNum
SMB::predictSourceStore(Addr load_pc)
{
  if (predictions.count(load_pc)) {
    return predictions[load_pc];
  }
  
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;

    std::stringstream ss(line);
    Addr l_pc, s_pc;
    if (!(ss >> l_pc >> s_pc)) continue;

    predictions[l_pc] = s_pc;

    if (l_pc == load_pc) {
      return s_pc;
    }
  }

  return 0;
}

} // namespace o3
} // namespace gem5
