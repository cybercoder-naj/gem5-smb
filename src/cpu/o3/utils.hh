/*
 * Copyright???
 */

#ifndef __CPU_O3_UTILS_HH__
#define __CPU_O3_UTILS_HH__

#include <utility>

#include "base/types.hh"

namespace gem5
{
namespace o3
{
  
using AddrSize = std::pair<Addr, unsigned>;

static inline
bool addrOverlap(AddrSize addr1, AddrSize addr2, unsigned depCheckShift = 0) {
  const Addr addr1_start = addr1.first >> depCheckShift;
  const Addr addr1_end = (addr1.first + addr1.second - 1) >> depCheckShift;
  const Addr addr2_start = addr2.first >> depCheckShift;
  const Addr addr2_end = (addr2.first + addr2.second - 1) >> depCheckShift;

  return addr1_start <= addr2_end && addr2_start <= addr1_end;
}

} // namespace o3
} // namespace gem5

#endif
