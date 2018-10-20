//===---- SDICABIInfo.cpp - Information about SDIC ABI's ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "SDICConfig.h"

#include "SDICABIInfo.h"
#include "SDICRegisterInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool>
EnableSDICS32Calls("sdic-s32-calls", cl::Hidden,
                    cl::desc("SDIC S32 call: use stack only to pass arguments.\
                    "), cl::init(false));

namespace {
static const MCPhysReg O32IntRegs[4] = {SDIC::R1, SDIC::R2};
static const MCPhysReg S32IntRegs = {};
}

const ArrayRef<MCPhysReg> SDICABIInfo::GetByValArgRegs() const {
  if (IsO32())
    return makeArrayRef(O32IntRegs);
  if (IsS32())
    return makeArrayRef(S32IntRegs);
  llvm_unreachable("Unhandled ABI");
}

const ArrayRef<MCPhysReg> SDICABIInfo::GetVarArgRegs() const {
  if (IsO32())
    return makeArrayRef(O32IntRegs);
  if (IsS32())
    return makeArrayRef(S32IntRegs);
  llvm_unreachable("Unhandled ABI");
}

unsigned SDICABIInfo::GetCalleeAllocdArgSizeInBytes(CallingConv::ID CC) const {
  if (IsO32())
    return CC != 0;
  if (IsS32())
    return 0;
  llvm_unreachable("Unhandled ABI");
}

SDICABIInfo SDICABIInfo::computeTargetABI() {
  SDICABIInfo abi(ABI::Unknown);

  if (EnableSDICS32Calls)
    abi = ABI::S32;
  else
    abi = ABI::O32;
  // Assert exactly one ABI was chosen.
  assert(abi.ThisABI != ABI::Unknown);

  return abi;
}

unsigned SDICABIInfo::GetStackPtr() const {
  return SDIC::R10;
}

unsigned SDICABIInfo::GetFramePtr() const {
  return SDIC::R12;
}

unsigned SDICABIInfo::GetNullPtr() const {
  return SDIC::R0;
}

unsigned SDICABIInfo::GetEhDataReg(unsigned I) const {
  static const unsigned EhDataReg[] = {
    SDIC::R0, SDIC::R1
  };

  return EhDataReg[I];
}

int SDICABIInfo::EhDataRegSize() const {
  if (ThisABI == ABI::S32)
    return 0;
  else
    return 2;
}

