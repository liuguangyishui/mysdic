//===-- SDICMCCodeEmitter.h - Convert SDIC Code to Machine Code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the SDICMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//
//

#ifndef LLVM_LIB_TARGET_SDIC_MCTARGETDESC_SDICMCCODEEMITTER_H
#define LLVM_LIB_TARGET_SDIC_MCTARGETDESC_SDICMCCODEEMITTER_H

#include "SDICConfig.h"

#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/Support/DataTypes.h"

using namespace llvm;

namespace llvm {
class MCContext;
class MCExpr;
class MCInst;
class MCInstrInfo;
class MCFixup;
class MCOperand;
class MCSubtargetInfo;
class raw_ostream;

 class SDICMCCodeEmitter : public MCCodeEmitter {
  SDICMCCodeEmitter(const SDICMCCodeEmitter &) = delete;
  void operator=(const SDICMCCodeEmitter &) = delete;
  const MCInstrInfo &MCII;
  MCContext &Ctx;
  bool IsLittleEndian;

  public:
  SDICMCCodeEmitter(const MCInstrInfo &mcii, MCContext &Ctx_, bool IsLittle)
      : MCII(mcii), Ctx(Ctx_), IsLittleEndian(IsLittle) {}

  ~SDICMCCodeEmitter() override {}

  // getJumpTargetOpValue - Return binary encoding of the jump
  // target operand, such as JSUB #function_addr. 
  // If the machine operand requires relocation,
  // record the relocation and return zero.
   unsigned getJumpTargetOpValue(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
   unsigned getExprOpValue(const MCExpr *Expr, SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const;
}; // class SDICMCCodeEmitter
} // namespace llvm.

#endif

