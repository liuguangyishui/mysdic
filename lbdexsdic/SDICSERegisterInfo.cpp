//===-- SDICSERegisterInfo.cpp - CPU0 Register Information ------== -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the CPU0 implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "SDICSERegisterInfo.h"

using namespace llvm;

#define DEBUG_TYPE "sdic-reg-info"

SDICSERegisterInfo::SDICSERegisterInfo(const SDICSubtarget &ST)
  : SDICRegisterInfo(ST) {}

const TargetRegisterClass *
SDICSERegisterInfo::intRegClass(unsigned Size) const {
  return &SDIC::SDICRegsRegClass;
}

