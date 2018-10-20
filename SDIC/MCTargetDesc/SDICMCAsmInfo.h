//===-- SDICMCAsmInfo.h - SDIC Asm Info ------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the SDICMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_MCTARGETDESC_SDICMCASMINFO_H
#define LLVM_LIB_TARGET_SDIC_MCTARGETDESC_SDICMCASMINFO_H

#include "SDICConfig.h"

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class Triple;

  class SDICMCAsmInfo : public MCAsmInfoELF {
    void anchor() override;
  public:
    explicit SDICMCAsmInfo(const Triple &TheTriple);
  };

} // namespace llvm

#endif

