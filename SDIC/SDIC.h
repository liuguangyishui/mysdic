//===-- SDIC.h - Top-level interface for SDIC representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM SDIC back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDIC_H
#define LLVM_LIB_TARGET_SDIC_SDIC_H

#include "SDICConfig.h"
#include "MCTargetDesc/SDICMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class SDICTargetMachine;
  class FunctionPass;

} // end namespace llvm;

#endif

