//===-- SDICInstrInfo.cpp - SDIC Instruction Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the SDIC implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "SDICInstrInfo.h"

#include "SDICTargetMachine.h"
#include "SDICMachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "SDICGenInstrInfo.inc"

// Pin the vtable to this file.
void SDICInstrInfo::anchor() {}

//@SDICInstrInfo {
SDICInstrInfo::SDICInstrInfo(const SDICSubtarget &STI)
    : 
      Subtarget(STI) {}

const SDICInstrInfo *SDICInstrInfo::create(SDICSubtarget &STI) {
  return llvm::createSDICSEInstrInfo(STI);
}

MachineMemOperand *
Cpu0InstrInfo::GetMemOperand(MachineBasicBlock &MBB, int FI,
                             MachineMemOperand::Flags Flags) const {

  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = *MF.getFrameInfo();
  unsigned Align = MFI.getObjectAlignment(FI);

  return MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(MF, FI),
                                 Flags, MFI.getObjectSize(FI), Align);
}

//@GetInstSizeInBytes {
/// Return the number of bytes of code the specified instruction may be.
unsigned SDICInstrInfo::GetInstSizeInBytes(const MachineInstr &MI) const {
//@GetInstSizeInBytes - body
  switch (MI.getOpcode()) {
  default:
    return MI.getDesc().getSize();
  }
}

