//===-- SDICSEFrameLowering.cpp - SDIC Frame Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the SDIC implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "SDICSEFrameLowering.h"

#include "SDICAnalyzeImmediate.h"
#include "SDICMachineFunction.h"
#include "SDICSEInstrInfo.h"
#include "SDICSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

SDICSEFrameLowering::SDICSEFrameLowering(const SDICSubtarget &STI)
    : SDICFrameLowering(STI, STI.stackAlignment()) {}

//@emitPrologue {
void SDICSEFrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  
  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");
  MachineFrameInfo *MFI = MF.getFrameInfo();
  SDICFunctionInfo *SDICFI = MF.getInfo<SDICFunctionInfo>();

  const SDICSEInstrInfo &TII =
    *static_cast<const SDICSEInstrInfo*>(STI.getInstrInfo());
  const SDICRegisterInfo &RegInfo =
    *static_cast<const SDICRegisterInfo *>(STI.getRegisterInfo());

  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  SDICABIInfo ABI = STI.getABI();
  unsigned SP = SDIC::R10;//STKPTR;
  const TargetRegisterClass *RC = &SDIC::GPROutRegClass;

  // First, compute final stack size.
  uint64_t StackSize = MFI->getStackSize();

  printf("the StackSize is %lu\n",StackSize);//hyl
  // No need to allocate space on the stack.
  if (StackSize == 0 && !MFI->adjustsStack()) return;

  MachineModuleInfo &MMI = MF.getMMI();
  const MCRegisterInfo *MRI = MMI.getContext().getRegisterInfo();
  MachineLocation DstML, SrcML;

  // Adjust stack.
  TII.adjustStackPtr(SP, StackSize, MBB, MBBI);

  // emit ".cfi_def_cfa_offset StackSize"
  unsigned CFIIndex = MMI.addFrameInst(
       MCCFIInstruction::createDefCfaOffset(nullptr, -StackSize));
  
  // printf("\nHYLHYL: whether the stacksize hasChanges %lu\n",StackSize);
  
  BuildMI(MBB, MBBI, dl, TII.get(TargetOpcode::CFI_INSTRUCTION))
    .addCFIIndex(CFIIndex);
  
  const std::vector<CalleeSavedInfo> &CSI = MFI->getCalleeSavedInfo();

  // printf("\nHYLHYL: THE CSI.size is %zu\n",CSI.size());
  
  if (CSI.size()) {
    // Find the instruction past the last instruction that saves a callee-saved
    // register to the stack.
    for (unsigned i = 0; i < CSI.size(); ++i)
      ++MBBI;
    // Iterate over list of callee-saved registers and emit .cfi_offset
    // directives.
    for (std::vector<CalleeSavedInfo>::const_iterator I = CSI.begin(),
	   E = CSI.end(); I != E; ++I) {
      int64_t Offset = MFI->getObjectOffset(I->getFrameIdx());
      unsigned Reg = I->getReg();
      {
// Reg is in CPURegs.
	unsigned CFIIndex = MMI.addFrameInst(MCCFIInstruction::createOffset(
	        	    nullptr, MRI->getDwarfRegNum(Reg, 1), Offset));
	BuildMI(MBB, MBBI, dl, TII.get(TargetOpcode::CFI_INSTRUCTION))
	  .addCFIIndex(CFIIndex);
      }
    }
    }
}
//}

//@emitEpilogue {
void SDICSEFrameLowering::emitEpilogue(MachineFunction &MF,
                                 MachineBasicBlock &MBB) const {
  
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  SDICFunctionInfo *SDICFI = MF.getInfo<SDICFunctionInfo>();
  const SDICSEInstrInfo &TII =
    *static_cast<const SDICSEInstrInfo *>(STI.getInstrInfo());
  const SDICRegisterInfo &RegInfo =
    *static_cast<const SDICRegisterInfo *>(STI.getRegisterInfo());
  DebugLoc dl = MBBI->getDebugLoc();
  SDICABIInfo ABI = STI.getABI();
  unsigned SP = SDIC::R10;//STKPTR;
  // Get the number of bytes from FrameInfo
  uint64_t StackSize = MFI->getStackSize();
  if (!StackSize)
    return;
  // Adjust stack.
  TII.adjustStackPtr(SP,-StackSize, MBB, MBBI);
}
//}

bool SDICSEFrameLowering::
spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                          MachineBasicBlock::iterator MI,
                          const std::vector<CalleeSavedInfo> &CSI,
                          const TargetRegisterInfo *TRI) const {
  MachineFunction *MF = MBB.getParent();
  MachineBasicBlock *EntryBlock = &MF->front();
  const TargetInstrInfo &TII = *MF->getSubtarget().getInstrInfo();

  for (unsigned i = 0, e = CSI.size(); i != e; ++i) {
    // Add the callee-saved register as live-in. Do not add if the register is
    // LR and return address is taken, because it has already been added in
    // method SDICTargetLowering::LowerRETURNADDR.
    // It's killed at the spill, unless the register is LR and return address
    // is taken.
    unsigned Reg = CSI[i].getReg();
    bool IsRAAndRetAddrIsTaken = (Reg == SDIC::R11)
        && MF->getFrameInfo()->isReturnAddressTaken();
    if (!IsRAAndRetAddrIsTaken)
      EntryBlock->addLiveIn(Reg);

    // Insert the spill to the stack frame.
    bool IsKill = !IsRAAndRetAddrIsTaken;
    const TargetRegisterClass *RC = TRI->getMinimalPhysRegClass(Reg);
    TII.storeRegToStackSlot(*EntryBlock, MI, Reg, IsKill,
                            CSI[i].getFrameIdx(), RC, TRI);
  }

  return true;
}


bool SDICSEFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  // Reserve call frame if the size of the maximum call frame fits into 16-bit
  // immediate field and there are no variable sized objects on the stack.
  // Make sure the second register scavenger spill slot can be accessed with one
  // instruction.
  return isInt<16>(MFI->getMaxCallFrameSize() + getStackAlignment()) &&
    !MFI->hasVarSizedObjects();
}

/// Mark \p Reg and all registers aliasing it in the bitset.
static void setAliasRegs(MachineFunction &MF, BitVector &SavedRegs, unsigned Reg) {
  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
  for (MCRegAliasIterator AI(Reg, TRI, true); AI.isValid(); ++AI)
    SavedRegs.set(*AI);
}

//@determineCalleeSaves {
// This method is called immediately before PrologEpilogInserter scans the 
//  physical registers used to determine what callee saved registers should be 
//  spilled. This method is optional. 
void SDICSEFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                               BitVector &SavedRegs,
                                               RegScavenger *RS) const {
  //@determineCalleeSaves-body
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  SDICFunctionInfo *SDICFI = MF.getInfo<SDICFunctionInfo>();
  MachineRegisterInfo& MRI = MF.getRegInfo();

  if (MF.getFrameInfo()->hasCalls())
    setAliasRegs(MF, SavedRegs, SDIC::R11);
  
  return;
}
//}
 
const SDICFrameLowering *
llvm::createSDICSEFrameLowering(const SDICSubtarget &ST) {
  return new SDICSEFrameLowering(ST);
}

