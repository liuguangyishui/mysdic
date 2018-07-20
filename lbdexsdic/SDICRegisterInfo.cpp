//===-- SDICRegisterInfo.cpp - SDIC Register Information -== --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the CPU0 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "sdic-reg-info"

#include "SDICRegisterInfo.h"

#include "SDIC.h"
#include "SDICSubtarget.h"
#include "SDICMachineFunction.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#define GET_REGINFO_TARGET_DESC
#include "SDICGenRegisterInfo.inc"

using namespace llvm;

SDICRegisterInfo::SDICRegisterInfo(const SDICSubtarget &ST)
  : SDICGenRegisterInfo(SDIC::R11), Subtarget(ST) {}

//===----------------------------------------------------------------------===//
// Callee Saved Registers methods
//===----------------------------------------------------------------------===//
/// SDIC Callee Saved Registers
// In SDICCallConv.td,
// def CSR_O32 : CalleeSavedRegs<(add LR, FP,
//                                   (sequence "S%u", 2, 0))>;
// llc create CSR_O32_SaveList and CSR_O32_RegMask from above defined.
const MCPhysReg *
SDICRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_O32_SaveList;
}

const uint32_t *
SDICRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const {
  return CSR_O32_RegMask; 
}

// pure virtual method
//@getReservedRegs {
BitVector SDICRegisterInfo::
getReservedRegs(const MachineFunction &MF) const {
//@getReservedRegs body {
  static const uint16_t ReservedCPURegs[] = {
    SDIC::R0, SDIC::TOSL, SDIC::STATUS, SDIC::STKPTR,
    SDIC::R10,SDIC::FSR0L, SDIC::FSR2L, SDIC::PCL, SDIC::R11,SDIC::PCH
  };
  BitVector Reserved(getNumRegs());

  for (unsigned I = 0; I < array_lengthof(ReservedCPURegs); ++I)
    Reserved.set(ReservedCPURegs[I]);

#ifdef ENABLE_GPRESTORE //1
  const SDICFunctionInfo *SDICFI = MF.getInfo<SDICFunctionInfo>();
  // Reserve GP if globalBaseRegFixed()
  if (SDICFI->globalBaseRegFixed())
#endif

  
   Reserved.set(SDIC::R3);
  
  return Reserved;
}

//@eliminateFrameIndex {
//- If no eliminateFrameIndex(), it will hang on run. 
// pure virtual method
// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
void SDICRegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                    unsigned FIOperandNum, RegScavenger *RS) const {

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  SDICFunctionInfo *SDICFI = MF.getInfo<SDICFunctionInfo>();

  unsigned i = 0;
  while (!MI.getOperand(i).isFI()) {
    ++i;
    assert(i < MI.getNumOperands() &&
	   "Instr doesn't have FrameIndex operand!");
  }
  DEBUG(errs() << "\nFunction : " << MF.getFunction()->getName() << "\n";
	errs() << "<--------->\n" << MI);
  
  int FrameIndex = MI.getOperand(i).getIndex();
  uint64_t stackSize = MF.getFrameInfo()->getStackSize();
  int64_t spOffset = MF.getFrameInfo()->getObjectOffset(FrameIndex);



  DEBUG(errs() << "FrameIndex : " << FrameIndex << "\n"
	<< "spOffset: " << spOffset << "\n"
	<< "stackSize : " << stackSize << "\n");
  
  const std::vector<CalleeSavedInfo> &CSI = MFI->getCalleeSavedInfo();
  int MinCSFI = 0;
  int MaxCSFI = -1;
  
  if (CSI.size()) {
    MinCSFI = CSI[0].getFrameIdx();
    MaxCSFI = CSI[CSI.size() - 1].getFrameIdx();
  }
 
  // The following stack frame objects are always referenced relative to $sp:
  // 1. Outgoing arguments.
  // 2. Pointer to dynamically allocated stack space.
  // 3. Locations for callee-saved registers.
  // Everything else is referenced relative to whatever register
  // getFrameRegister() returns.
  unsigned FrameReg;
  FrameReg = SDIC::R10;

  // Calculate final offset.
  // - There is no need to change the offset if the frame object is one of the
  //following: an outgoing argument, pointer to a dynamically allocated
  // stack space or a $gp restore location,
  // - If the frame object is any of the following, its offset must be adjusted
  //by adding the size of the stack:
  //incoming argument, callee-saved register location or local variable.
  int64_t Offset;
  Offset = spOffset + (int64_t)stackSize;
  Offset  += MI.getOperand(i+1).getImm();

  //HYL
  Offset = FrameIndex+ 0X01;
  
  DEBUG(errs() << "Offset: " << Offset << "\n" << "<--------->\n");
  // If MI is not a debug value, make sure Offset fits in the 16-bit immediate
  // field.
  if (!MI.isDebugValue() && !isInt<16>(Offset)) {
    assert("(!MI.isDebugValue() && !isInt<16>(Offset))");
  }
  
   MI.getOperand(i).ChangeToRegister(FrameReg, false);
   MI.getOperand(i+1).ChangeToImmediate(Offset);
  //MI.getOperand(i).ChangeToImmediate(Offset);
 
}

//}

bool
SDICRegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool
SDICRegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

// pure virtual method
unsigned SDICRegisterInfo::
getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  return TFI->hasFP(MF) ? (SDIC::R12) :
                          (SDIC::R10);
}

