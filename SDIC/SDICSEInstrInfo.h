//===-- SDICSEInstrInfo.h - SDIC32/64 Instruction Information ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the SDIC32/64 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDICSEINSTRINFO_H
#define LLVM_LIB_TARGET_SDIC_SDICSEINSTRINFO_H

#include "SDICConfig.h"

#include "SDICInstrInfo.h"
#include "SDICSERegisterInfo.h"
#include "SDICMachineFunction.h"

namespace llvm {
  
class SDICSEInstrInfo : public SDICInstrInfo {
  const SDICSERegisterInfo RI;

public:
 
  
  explicit SDICSEInstrInfo(const SDICSubtarget &STI);

  const SDICRegisterInfo &getRegisterInfo() const override;

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
		   const DebugLoc &DL, unsigned DestReg, unsigned SrcReg,
                   bool KillSrc) const override;
  
  void storeRegToStack(MachineBasicBlock &MBB,
                       MachineBasicBlock::iterator MI,
                       unsigned SrcReg, bool isKill, int FrameIndex,
                       const TargetRegisterClass *RC,
                       const TargetRegisterInfo *TRI,
                       int64_t Offset) const override;

  void loadRegFromStack(MachineBasicBlock &MBB,
                        MachineBasicBlock::iterator MI,
                        unsigned DestReg, int FrameIndex,
                        const TargetRegisterClass *RC,
                        const TargetRegisterInfo *TRI,
                        int64_t Offset) const override;

  //@expandPostRAPseudo
  bool expandPostRAPseudo( MachineInstr &MI) const override;

  /// Adjust SP by Amount bytes.
  void adjustStackPtr(unsigned SP, int64_t Amount, MachineBasicBlock &MBB,
		      MachineBasicBlock::iterator I) const override;

  /// Emit a series of instructions to load an immediate. If NewImm is a
  /// non-NULL parameter, the last instruction is not emitted, but instead
  /// its immediate operand is returned in NewImm.
  unsigned loadImmediate(int64_t Imm, MachineBasicBlock &MBB,
			 MachineBasicBlock::iterator II, const DebugLoc &DL,
			 unsigned *NewImm) const;

private:
  void expandRetLR(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const;
  
};

}

#endif

