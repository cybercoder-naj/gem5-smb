/*
 * Copyright... me!
 */

#include "cpu/o3/bypass_move_inst.hh"

#include "cpu/exec_context.hh"
#include "cpu/static_inst.hh"

namespace gem5
{

namespace o3
{

/**
 * The requirements of the move is:
 * 1. store data register - to get the data the store is writing.
 * 2. store execution - to get the effSize of the request. (this is the major limitation of gem5)
 * 3. Previous physical register - to get the existing value of the arch_reg before the load (so to bitmask correctly).
 * 
 * This will not have any RegIds retrievable from `
 */
class BypassMoveStaticInst : public StaticInst
{
  private:
    // Declare the arrays as members of this class.
    // The size must be known at compile time.
    RegId _srcRegIdx[2];
    RegId _destRegIdx[1];
    uint64_t _destRegMask;

    bool isPartialWrite() const {
      return _destRegMask != UINT64_MAX;
    }

  public:
    BypassMoveStaticInst(RegId store_src, RegId load_src, 
      RegId load_dest, uint64_t destRegMask) : StaticInst("gem5 bypass move", No_OpClass), _destRegMask(destRegMask) {

      /*
       * This may be 1, if we knew the register width of store before hand.
       * So we keep it 2 just to be safe.
       * 
       * The two regs are
       * Idx 0. the store's value src register.
       * Idx 1. the load's previous dest physical register.
       */ 
      _numSrcRegs = isPartialWrite() ? 2 : 1; 
      
      /*
       * Reads from both srcs to write to the load's new dest physical register.
       * Idx 0. Load's new physical register.
       */
      _numDestRegs = 1;

      _srcRegIdx[0] = store_src;
      if (isPartialWrite())
        _srcRegIdx[1] = load_src;
      _destRegIdx[0] = load_dest;

      // Cast the member pointers up to StaticInst::*[] type
      setRegIdxArrays(
          reinterpret_cast<RegIdArrayPtr>(&BypassMoveStaticInst::_srcRegIdx),
          reinterpret_cast<RegIdArrayPtr>(&BypassMoveStaticInst::_destRegIdx)
      );
    }

    Fault execute(ExecContext *xc, trace::InstRecord *traceData) const override
    {
      RegVal result = 0;

      // Read source operands
      RegVal src_data = xc->getRegOperand(this, 0);  // First operand
      if (isPartialWrite()) {
        RegVal prev_load = xc->getRegOperand(this, 1);  // Second operand

        result = (src_data & _destRegMask) | (prev_load & ~_destRegMask);
      } else 
        result = src_data;

      // Write back result
      xc->setRegOperand(this, 0, result);  // Write to dest register (index 0)
      
      return NoFault;      
    }

    void
    advancePC(PCStateBase &pc) const override
    {
      pc.advance();
    }

    std::string
    generateDisassembly(Addr pc,
      const loader::SymbolTable *symtab) const override
    {
      return mnemonic;
    }
};

StaticInstPtr
buildBypassMoveStaticInst(RegId store_src, RegId load_src, RegId load_dest, uint64_t destRegMask) 
{
  return new BypassMoveStaticInst(store_src, load_src, load_dest, destRegMask);  
}

} // namespace o3

} // namespace gem5
