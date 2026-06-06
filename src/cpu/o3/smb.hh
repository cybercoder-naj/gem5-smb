/*
 * Copyright ??
 */

#ifndef __CPU_O3_SMB_HH__
#define __CPU_O3_SMB_HH__

#include <cstdint>
#include <fstream>
#include <optional>
#include <string>

#include "base/types.hh"
#include "cpu/o3/dyn_inst_ptr.hh"
#include "cpu/o3/mascot.hh"

namespace gem5
{

namespace o3
{

struct PredictionEntry {
  std::optional<InstSeqNum> instSeqNum;
  Addr loadPC;
  uint64_t sqDist;
};

/** The speculative memory bypass predictor. */
class SMB
{
  private:
    /** The object name, for DPRINTF. */
    const std::string _name;
    std::fstream infile;

    std::deque<PredictionEntry> predictions;
    std::deque<PredictionEntry>::iterator dispatchIt;

  public:
    /** Constructor. */
    SMB(const std::string &_my_name);

    /** Destructor. */
    ~SMB() {}

    std::string name() const { return _name; }

    MASCOT::Prediction predict(Addr load_pc, InstSeqNum load_seq_num);

    bool nextPrediction();

    void squash(InstSeqNum squashed_seq_num); 

    void removeUpTo(InstSeqNum seq_num);

    bool deletePrediction(Addr load_addr);
};

} // namespace o3
} // namespace gem5


#endif
