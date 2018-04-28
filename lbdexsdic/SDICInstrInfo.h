//===-- SDICInstrInfo.h - SDIC Instruction Information ----------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_SDIC_SDICINSTRINFO_H
#define LLVM_LIB_TARGET_SDIC_SDICINSTRINFO_H


#include "SDICConfig.h"

#include "SDIC.h"
#include "SDICAnalyzeImmediate.h"
#include "SDICRegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "SDICGenInstrInfo.inc"

namespace llvm {

class SDICInstrInfo : public SDICGenInstrInfo {
  virtual void anchor();
protected:
  const SDICSubtarget &Subtarget;
public:
  explicit SDICInstrInfo(const SDICSubtarget &STI);

  static const SDICInstrInfo *create(SDICSubtarget &STI);

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  virtual const SDICRegisterInfo &getRegisterInfo() const = 0;

  /// Return the number of bytes of code the specified instruction may be.
  unsigned GetInstSizeInBytes(const MachineInstr &MI) const;

  virtual void adjustStackPtr(unsigned SP, int64_t Amount,
			      MachineBasicBlock &MBB,
			      MachineBasicBlock::iterator I) const = 0;
  

protected:
};
const SDICInstrInfo *createSDICSEInstrInfo(const SDICSubtarget &STI);
}

#endif

