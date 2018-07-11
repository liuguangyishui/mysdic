//===-- SDICMCCodeEmitter.cpp - Convert SDIC Code to Machine Code ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the SDICMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//
//

#include "SDICMCCodeEmitter.h"

#include "MCTargetDesc/SDICBaseInfo.h"
#include "MCTargetDesc/SDICFixupKinds.h"
#include "MCTargetDesc/SDICMCExpr.h"
#include "MCTargetDesc/SDICMCTargetDesc.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "mccodeemitter"

#define GET_INSTRMAP_INFO
#include "SDICGenInstrInfo.inc"
#undef GET_INSTRMAP_INFO


/// getJumpTargetOpValue - Return binary encoding of the jump
/// target operand, such as JSUB. 
/// If the machine operand requires relocation,
/// record the relocation and return zero.
//@getJumpTargetOpValue {
unsigned SDICMCCodeEmitter::
getJumpTargetOpValue(const MCInst &MI, unsigned OpNo,
                     SmallVectorImpl<MCFixup> &Fixups,
                     const MCSubtargetInfo &STI) const {
  unsigned Opcode = MI.getOpcode();
  const MCOperand &MO = MI.getOperand(OpNo);
  // If the destination is an immediate, we have nothing to do.
  if (MO.isImm()) return MO.getImm();
  assert(MO.isExpr() && "getJumpTargetOpValue expects only expressions");

  const MCExpr *Expr = MO.getExpr();
  if (Opcode == SDIC::JSUB || Opcode == SDIC::BRA || Opcode == SDIC::BAL)
    Fixups.push_back(MCFixup::create(0, Expr,
                                     MCFixupKind(SDIC::fixup_SDIC_PC24)));
  else
    llvm_unreachable("unexpect opcode in getJumpAbsoluteTargetOpValue()");
  return 0;
}
//@CH8_1 }

//@getExprOpValue {
unsigned SDICMCCodeEmitter::
getExprOpValue(const MCExpr *Expr,SmallVectorImpl<MCFixup> &Fixups,
               const MCSubtargetInfo &STI) const {
//@getExprOpValue body {
  MCExpr::ExprKind Kind = Expr->getKind();
  if (Kind == MCExpr::Constant) {
    return cast<MCConstantExpr>(Expr)->getValue();
  }

  if (Kind == MCExpr::Binary) {
    unsigned Res = getExprOpValue(cast<MCBinaryExpr>(Expr)->getLHS(), Fixups, STI);
    Res += getExprOpValue(cast<MCBinaryExpr>(Expr)->getRHS(), Fixups, STI);
    return Res;
  }

  if (Kind == MCExpr::Target) {
    const SDICMCExpr *SDICExpr = cast<SDICMCExpr>(Expr);

    SDIC::Fixups FixupKind = SDIC::Fixups(0);
    switch (SDICExpr->getKind()) {
    default: llvm_unreachable("Unsupported fixup kind for target expression!");
  //@switch {
//    switch(cast<MCSymbolRefExpr>(Expr)->getKind()) {
  //@switch }
    case SDICMCExpr::CEK_GPREL:
      FixupKind = SDIC::fixup_SDIC_GPREL16;
      break;
    case SDICMCExpr::CEK_GOT_CALL:
      FixupKind = SDIC::fixup_SDIC_CALL16;
      break;
    case SDICMCExpr::CEK_GOT:
      FixupKind = SDIC::fixup_SDIC_GOT;
      break;
    case SDICMCExpr::CEK_ABS_HI:
      FixupKind = SDIC::fixup_SDIC_HI16;
      break;
    case SDICMCExpr::CEK_ABS_LO:
      FixupKind = SDIC::fixup_SDIC_LO16;
      break;
    case SDICMCExpr::CEK_GOT_HI16:
      FixupKind = SDIC::fixup_SDIC_GOT_HI16;
      break;
    case SDICMCExpr::CEK_GOT_LO16:
      FixupKind = SDIC::fixup_SDIC_GOT_LO16;
      break;
    } // switch
    Fixups.push_back(MCFixup::create(0, Expr, MCFixupKind(FixupKind)));
    return 0;
  }
   // All of the information is in the fixup.
  return 0;
}
