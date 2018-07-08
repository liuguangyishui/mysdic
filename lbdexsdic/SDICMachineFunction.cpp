//===-- SDICMachineFunctionInfo.cpp - Private data used for SDIC ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "SDICMachineFunction.h"

#include "MCTargetDesc/SDICBaseInfo.h"
#include "SDICInstrInfo.h"
#include "SDICSubtarget.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

bool FixGlobalBaseReg;

SDICFunctionInfo::~SDICFunctionInfo() {}


unsigned SDICFunctionInfo::getGlobalBaseReg() {
  return GlobalBaseReg = SDIC::R10;
}

MachinePointerInfo SDICFunctionInfo::callPtrInfo(const char *ES) {
  return MachinePointerInfo(MF.getPSVManager().getExternalSymbolCallEntry(ES));
}

MachinePointerInfo SDICFunctionInfo::callPtrInfo(const GlobalValue *GV) {
  return MachinePointerInfo(MF.getPSVManager().getGlobalValueCallEntry(GV));
}

void SDICFunctionInfo::anchor() {
 
}
