//===-- llvm/Target/SDICTargetObjectFile.h - SDIC Object Info ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDICTARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_SDIC_SDICTARGETOBJECTFILE_H

#include "SDICConfig.h"

#include "SDICTargetMachine.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class SDICTargetMachine;
  class SDICTargetObjectFile : public TargetLoweringObjectFileELF {
    MCSection *SmallDataSection;
    MCSection *SmallBSSSection;
    const SDICTargetMachine *TM;
  public:

    void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

    /// IsGlobalInSmallSection - Return true if this global address should be
    /// placed into small data/bss section.
    bool IsGlobalInSmallSection(const GlobalValue *GV,
                                const TargetMachine &TM, SectionKind Kind) const;
    bool IsGlobalInSmallSection(const GlobalValue *GV,
                                const TargetMachine &TM) const;
    bool IsGlobalInSmallSectionImpl(const GlobalValue *GV,
                                    const TargetMachine &TM) const;

    MCSection *SelectSectionForGlobal(const GlobalValue *GV, SectionKind Kind,
                                      Mangler &Mang,
                                      const TargetMachine &TM) const override;

  };
} // end namespace llvm

#endif

