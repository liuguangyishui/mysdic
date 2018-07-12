//===-- SDICAsmBackend.h - SDIC Asm Backend  ------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the SDICAsmBackend class.
//
//===----------------------------------------------------------------------===//
//

#ifndef LLVM_LIB_TARGET_SDIC_MCTARGETDESC_SDICASMBACKEND_H
#define LLVM_LIB_TARGET_SDIC_MCTARGETDESC_SDICASMBACKEND_H

#include "SDICConfig.h"

#include "MCTargetDesc/SDICFixupKinds.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmBackend.h"

namespace llvm {
  
  class MCAssembler;
  struct MCFixupKindInfo;
  class Target;
  class MCObjectWriter;

  class SDICAsmBackend : public MCAsmBackend {
    Triple::OSType OSType;
    bool IsLittle; // Big or little endian

public:
     SDICAsmBackend(const Target &T, Triple::OSType _OSType, bool IsLittle)
      : MCAsmBackend(), OSType(_OSType), IsLittle(IsLittle) {}
  
    MCObjectWriter *createObjectWriter(raw_pwrite_stream &OS) const override;


  }
}

#endif
