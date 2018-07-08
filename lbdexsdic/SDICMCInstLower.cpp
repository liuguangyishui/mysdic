//===-- SDICMCInstLower.cpp - Convert SDIC MachineInstr to MCInst ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower SDIC MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "SDICMCInstLower.h"

#include "SDICAsmPrinter.h"
#include "SDICInstrInfo.h"
#include "MCTargetDesc/SDICBaseInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"

using namespace llvm;

SDICMCInstLower::SDICMCInstLower(SDICAsmPrinter &asmprinter)
  : AsmPrinter(asmprinter) {}

void SDICMCInstLower::Initialize(MCContext* C) {
  Ctx = C;
}

//@LowerSymbolOperand {
MCOperand SDICMCInstLower::LowerSymbolOperand(const MachineOperand &MO,
					      MachineOperandType MOTy,
					      unsigned Offset) const {
  MCSymbolRefExpr::VariantKind Kind = MCSymbolRefExpr::VK_None;
  SDICMCExpr::SDICExprKind TargetKind = SDICMCExpr::CEK_None;
  const MCSymbol *Symbol;

  switch(MO.getTargetFlags()) {
  default:            llvm_unreachable("Invalid target flag!");
  case SDICII::MO_NO_FLAG:
    break;

// SDIC_GPREL is for llc -march=cpu0 -relocation-model=static -cpu0-islinux-
//  format=false (global var in .sdata).
  case SDICII::MO_GPREL:
    TargetKind = SDICMCExpr::CEK_GPREL;
    break;

    case SDICII::MO_GOT:
    TargetKind = SDICMCExpr::CEK_GOT;
    break;
// ABS_HI and ABS_LO is for llc -march=cpu0 -relocation-model=static (global 
//  var in .data).
  case SDICII::MO_ABS_HI:
    TargetKind = SDICMCExpr::CEK_ABS_HI;
    break;
  case SDICII::MO_ABS_LO:
    TargetKind = SDICMCExpr::CEK_ABS_LO;
    break;
  case SDICII::MO_GOT_HI16:
    TargetKind = SDICMCExpr::CEK_GOT_HI16;
    break;
  case SDICII::MO_GOT_LO16:
    TargetKind = SDICMCExpr::CEK_GOT_LO16;
    break;
    
  case SDICII::MO_GOT_CALL:
    TargetKind = SDICMCExpr::CEK_GOT_CALL;
    break;

  }

  switch (MOTy) {
  case MachineOperand::MO_GlobalAddress:
    Symbol = AsmPrinter.getSymbol(MO.getGlobal());
    Offset += MO.getOffset();
    break;

  case MachineOperand::MO_MachineBasicBlock:
    Symbol = MO.getMBB() -> getSymbol();
    break;

  case MachineOperand::MO_BlockAddress:
    Symbol = AsmPrinter.GetBlockAddressSymbol(MO.getBlockAddress());
    Offset += MO.getOffset();
    break;

  case MachineOperand::MO_ExternalSymbol:
    Symbol = AsmPrinter.GetExternalSymbolSymbol(MO.getSymbolName());
    Offset += MO.getOffset();
    break;
    
  case MachineOperand::MO_JumpTableIndex:
    Symbol = AsmPrinter.GetJTISymbol(MO.getIndex());
    break;

  default:
    llvm_unreachable("<unknown operand type>");
  }

  const MCExpr *Expr = MCSymbolRefExpr::create(Symbol, Kind, *Ctx);

  if(Offset) {
    //Assume offset is never negative
    assert(Offset > 0);
    Expr = MCBinaryExpr::createAdd(Expr, MCConstantExpr::create(Offset, *Ctx),
				   *Ctx);
  }

  if(TargetKind != SDICMCExpr::CEK_None)
    Expr = SDICMCExpr::create(TargetKind, Expr, *Ctx);

  return MCOperand::createExpr(Expr);

}

static void CreateMCInst(MCInst& Inst, unsigned Opc, const MCOperand& Opnd0,
                         const MCOperand& Opnd1,
                         const MCOperand& Opnd2 = MCOperand()) {
  Inst.setOpcode(Opc);
  Inst.addOperand(Opnd0);
  Inst.addOperand(Opnd1);
  if (Opnd2.isValid())
    Inst.addOperand(Opnd2);
}

//@LowerOperand {
MCOperand SDICMCInstLower::LowerOperand(const MachineOperand& MO,
                                        unsigned offset) const {
  MachineOperandType MOTy = MO.getType();

  switch (MOTy) {
  //@2
  default: llvm_unreachable("unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit()) break;
    return MCOperand::createReg(MO.getReg());
  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm() + offset);

  case MachineOperand::MO_MachineBasicBlock:
  case MachineOperand::MO_ExternalSymbol:
  case MachineOperand::MO_JumpTableIndex:
  case MachineOperand::MO_BlockAddress:
  case MachineOperand::MO_GlobalAddress:
    return LowerSymbolOperand(MO, MOTy, offset);

  case MachineOperand::MO_FrameIndex:
    printf("this is MO_FrameIndex");
    break;
    
    //@1
  case MachineOperand::MO_RegisterMask:
    break;
 }

  return MCOperand();
}

void SDICMCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI->getOperand(i);
    MCOperand MCOp = LowerOperand(MO);

    if (MCOp.isValid())
      OutMI.addOperand(MCOp);
  }
}

