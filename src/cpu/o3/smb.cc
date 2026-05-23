/*
 * Copyright ??
 */

#include "cpu/o3/smb.hh"


#include "base/trace.hh"
#include "cpu/o3/dyn_inst_ptr.hh"
#include "cpu/o3/mascot.hh"
#include "debug/SMB.hh"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <string>

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

  predIdx = 0;
}

MASCOT::Prediction
SMB::predict(Addr load_pc, InstSeqNum inst_seq_num, BranchHistory branch_history)
{
  MASCOT::Prediction pred {
    .type = MASCOT::PredictionType::SMB,
    .distances = {0, 0},
    .tableIdx = -1u,
    .hash = 0,
  };

  if (predIdx >= predictions.size())
    if (!nextPrediction()) {
      return pred;
    } 

  auto &pred_entry = predictions[predIdx++];
  assert(pred_entry.loadPC == load_pc);
  assert(!pred_entry.instSeqNum.has_value());
  pred_entry.instSeqNum = inst_seq_num;

  pred.distances.first = pred_entry.sqDist; 

  return pred;
}

bool
SMB::nextPrediction() {
  DPRINTF(SMB, "Getting line number %i", predIdx + 1);

  std::string line;
  while(std::getline(infile, line)) {
    if (line.empty()) continue;  

    PredictionEntry entry {
      .instSeqNum = std::nullopt,
      .loadPC = 0,
      .sqDist = 0
    };

    if (sscanf(line.c_str(), "%" SCNx64 " %" SCNu64, &entry.loadPC, &entry.sqDist) != 2) continue;

    predictions.push_back(entry);
    return true;
  }

  infile.close();
  return false;
}

void
SMB::squash(InstSeqNum squashed_seq_num)
{
  while (predIdx - 1 >= 0 &&
          predictions[predIdx - 1].instSeqNum > squashed_seq_num)
    --predIdx;
}

void 
SMB::removeUpTo(InstSeqNum seq_num)
{
  for (auto it = predictions.begin(); it != predictions.end(); ) {
    if (it->instSeqNum.has_value() && it->instSeqNum.value() <= seq_num)
      it = predictions.erase(it);
    else break;
  }
}

bool
SMB::deletePrediction(Addr load_addr)
{
  return false;
}

} // namespace o3
} // namespace gem5
