/*
 * Copyright ??
 */

#ifndef __CPU_O3_SMB_HH__
#define __CPU_O3_SMB_HH__

#include <string>
#include <unordered_map>

#include "cpu/o3/dyn_inst_ptr.hh"

namespace gem5
{

namespace o3
{

/** The speculative memory bypass predictor. */
class SMB
{
  private:
    /** The object name, for DPRINTF. */
    const std::string _name;

    std::unordered_map<InstSeqNum, InstSeqNum> predictions;

    void loadPredictions();

  public:
    /** Constructor. */
    SMB(const std::string &_my_name);

    /** Destructor. */
    ~SMB() {}

    std::string name() const { return _name; }

    InstSeqNum predictSourceStore(InstSeqNum load_seq_num);
};

} // namespace o3
} // namespace gem5


#endif