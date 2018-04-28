//===-- SDICFrameLowering.h - Define frame lowering for SDIC ----*- C++ -*-===//
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
#ifndef LLVM_LIB_TARGET_SDIC_SDICFRAMELOWERING_H
#define LLVM_LIB_TARGET_SDIC_SDICFRAMELOWERING_H

#include "SDICConfig.h"

#include "SDIC.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
  class SDICSubtarget;

class SDICFrameLowering : public TargetFrameLowering {
protected:
  const SDICSubtarget &STI;

public:
  explicit SDICFrameLowering(const SDICSubtarget &sti, unsigned Alignment)
    : TargetFrameLowering(StackGrowsDown, Alignment, 0, Alignment),
      STI(sti) {
  }

  static const SDICFrameLowering *create(const SDICSubtarget &ST);

  bool hasFP(const MachineFunction &MF) const override;

};

/// Create SDICFrameLowering objects.
const SDICFrameLowering *createSDICSEFrameLowering(const SDICSubtarget &ST);

} // End llvm namespace

#endif

