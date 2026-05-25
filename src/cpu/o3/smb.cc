/*
 * Copyright ??
 */

#include "cpu/o3/smb.hh"


#include "base/debug.hh"
#include "base/trace.hh"
#include "cpu/o3/dyn_inst_ptr.hh"
#include "cpu/o3/mascot.hh"
#include "debug/SMB.hh"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <optional>
#include <string>

namespace gem5
{

namespace o3
{

SMB::SMB(const std::string &_my_name) :
    _name(_my_name),
    dispatchIt(predictions.end())
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

MASCOT::Prediction
SMB::predict(Addr load_pc, InstSeqNum inst_seq_num, BranchHistory branch_history)
{
  MASCOT::Prediction pred {
    .type = MASCOT::PredictionType::NDEP,
    .distances = {0, 0},
    .tableIdx = -1u,
    .hash = 0,
  };

  if (dispatchIt == predictions.end()) {
    if (!nextPrediction())
      return pred;
    dispatchIt = std::prev(predictions.end());
  }

  assert(dispatchIt->loadPC == load_pc);
  assert(!dispatchIt->instSeqNum.has_value());

  dispatchIt->instSeqNum = inst_seq_num;

  if (dispatchIt->sqDist > 0)
    pred.type = MASCOT::PredictionType::SMB;
  pred.distances.first = dispatchIt->sqDist; 

  ++dispatchIt;

  return pred;
}

bool
SMB::nextPrediction() {
  DPRINTF(SMB, "Getting new line \n");

  std::string line;
  while(std::getline(infile, line)) {
    if (line.empty()) continue;  

    PredictionEntry entry {
      .instSeqNum = std::nullopt,
      .loadPC = 0,
      .sqDist = 0
    };

    if (sscanf(line.c_str(), 
               "%" SCNx64 " %" SCNu64, 
               &entry.loadPC,
               &entry.sqDist) != 2) 
      continue;

    predictions.push_back(entry);
    return true;
  }

  infile.close();
  return false;
}

void
SMB::squash(InstSeqNum squashed_seq_num)
{
  while (dispatchIt != predictions.begin()) {
    auto prev = std::prev(dispatchIt);
    if (!prev->instSeqNum.has_value() ||
        prev->instSeqNum.value() <= squashed_seq_num)
      break;
    prev->instSeqNum = std::nullopt;
    dispatchIt = prev;
  }
}

void 
SMB::removeUpTo(InstSeqNum seq_num)
{
  while (!predictions.empty()) {
    auto &front = predictions.front();
    if (!front.instSeqNum.has_value() ||
        front.instSeqNum.value() > seq_num)
      break;

    assert(dispatchIt != predictions.begin());
    predictions.pop_front();
  }
}

bool
SMB::deletePrediction(Addr load_addr)
{
  return false;
}

} // namespace o3
} // namespace gem5
