/*
 * Copyright... me!
 */

#ifndef __CPU_BYPASS_MOVE_STATIC_INST_HH__
#define __CPU_BYPASS_MOVE_STATIC_INST_HH__

#include "cpu/reg_class.hh"
#include "cpu/static_inst_fwd.hh"

namespace gem5
{

namespace o3
{

StaticInstPtr buildBypassMoveStaticInst(RegId store_src, RegId load_src, RegId load_dest);
  
} // namespace o3
} // namespace gem5

#endif // __CPU_BYPASS_MOVE_STATIC_INST_HH__