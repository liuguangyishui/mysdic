//===-- Cpu0ELFObjectWriter.cpp - Cpu0 ELF Writer -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Cpu0Config.h"

#include "MCTargetDesc/Cpu0BaseInfo.h"
#include "MCTargetDesc/Cpu0FixupKinds.h"
#include "MCTargetDesc/Cpu0MCTargetDesc.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <list>

using namespace llvm;

namespace {
  class Cpu0ELFObjectWriter : public MCELFObjectTargetWriter {
  public:
    Cpu0ELFObjectWriter(uint8_t OSABI);

    ~Cpu0ELFObjectWriter() override;

    unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
    bool needsRelocateWithSymbol(const MCSymbol &Sym,
                                 unsigned Type) const override;
  };
}

Cpu0ELFObjectWriter::Cpu0ELFObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*_is64Bit=false*/ false, OSABI, ELF::EM_CPU0,
                            /*HasRelocationAddend*/ false) {}

Cpu0ELFObjectWriter::~Cpu0ELFObjectWriter() {}

//@GetRelocType {
unsigned Cpu0ELFObjectWriter::getRelocType(MCContext &Ctx,
                                           const MCValue &Target,
                                           const MCFixup &Fixup,
                                           bool IsPCRel) const {
  // determine the type of the relocation
  unsigned Type = (unsigned)ELF::R_CPU0_NONE;
  unsigned Kind = (unsigned)Fixup.getKind();

  switch (Kind) {
  default:
    llvm_unreachable("invalid fixup kind!");

  case Cpu0::fixup_Cpu0_CALL16:
    Type = ELF::R_CPU0_CALL16;
    break;
  }
  return Type;
}
//@GetRelocType }


MCObjectWriter *llvm::createCpu0ELFObjectWriter(raw_pwrite_stream &OS,
                                                uint8_t OSABI,
                                                bool IsLittleEndian) {
  MCELFObjectTargetWriter *MOTW = new Cpu0ELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, IsLittleEndian);
}
