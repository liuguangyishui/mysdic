 //===-- SDICISEISelLowering.h - SDICISE DAG Lowering Interface ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of SDICITargetLowering specialized for cpu032/64.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDICSEISELLOWERING_H
#define LLVM_LIB_TARGET_SDIC_SDICSEISELLOWERING_H

#include "SDICConfig.h"

#include "SDICISelLowering.h"
#include "SDICRegisterInfo.h"

namespace llvm {
  class SDICSETargetLowering : public SDICTargetLowering  {
  public:
    explicit SDICSETargetLowering(const SDICTargetMachine &TM,
                                  const SDICSubtarget &STI);

    SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;
  private:
  };
}

#endif // SDIC  ISEISELLOWERING_H

