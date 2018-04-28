//===-- SDICMCTargetDesc.h - SDIC Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides SDIC specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_MCTARGETDESC_SDICMCTARGETDESC_H
#define LLVM_LIB_TARGET_SDIC_MCTARGETDESC_SDICMCTARGETDESC_H

#include "SDICConfig.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
class Target;
class Triple;
class raw_ostream;
class raw_pwrite_stream;

extern Target TheSDICTarget;
extern Target TheSDICelTarget;

} // End llvm namespace

// Defines symbolic names for SDIC registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "SDICGenRegisterInfo.inc"

// Defines symbolic names for the SDIC instructions.
#define GET_INSTRINFO_ENUM
#include "SDICGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "SDICGenSubtargetInfo.inc"

#endif

