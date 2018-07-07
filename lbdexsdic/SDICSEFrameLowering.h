//===-- SDICSEFrameLowering.h - SDIC32/64 frame lowering --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDICSEFRAMELOWERING_H
#define LLVM_LIB_TARGET_SDIC_SDICSEFRAMELOWERING_H

#include "SDICConfig.h"

#include "SDICFrameLowering.h"

namespace llvm {

class SDICSEFrameLowering : public SDICFrameLowering {
public:
  explicit SDICSEFrameLowering(const SDICSubtarget &STI);

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 const std::vector<CalleeSavedInfo> &CSI,
                                 const TargetRegisterInfo *TRI) const override;
  
  bool hasReservedCallFrame(const MachineFunction &MF) const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
			    RegScavenger *RS) const override;
  

};

} // End llvm namespace

#endif

