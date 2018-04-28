//===-- SDICSERegisterInfo.h - SDIC32 Register Information ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the SDIC32/64 implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDICSEREGISTERINFO_H
#define LLVM_LIB_TARGET_SDIC_SDICSEREGISTERINFO_H

#include "SDICConfig.h"

#include "SDICRegisterInfo.h"

namespace llvm {
class SDICSEInstrInfo;

class SDICSERegisterInfo : public SDICRegisterInfo {
public:
  SDICSERegisterInfo(const SDICSubtarget &Subtarget);

  const TargetRegisterClass *intRegClass(unsigned Size) const override;
};

} // end namespace llvm

#endif

