//===-- SDICELFObjectWriter.cpp - SDIC ELF Writer -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "SDICConfig.h"

#include "MCTargetDesc/SDICBaseInfo.h"
#include "MCTargetDesc/SDICFixupKinds.h"
#include "MCTargetDesc/SDICMCTargetDesc.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <list>

using namespace llvm;

namespace {
  class SDICELFObjectWriter : public MCELFObjectTargetWriter {
  public:
    SDICELFObjectWriter(uint8_t OSABI);

    ~SDICELFObjectWriter() override;

    unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
    bool needsRelocateWithSymbol(const MCSymbol &Sym,
                                 unsigned Type) const override;
  };
}

SDICELFObjectWriter::SDICELFObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*_is64Bit=false*/ false, OSABI, ELF::EM_SDIC,
                            /*HasRelocationAddend*/ false) {}

SDICELFObjectWriter::~SDICELFObjectWriter() {}

//@GetRelocType {
unsigned SDICELFObjectWriter::getRelocType(MCContext &Ctx,
                                           const MCValue &Target,
                                           const MCFixup &Fixup,
                                           bool IsPCRel) const {
  // determine the type of the relocation
  unsigned Type = (unsigned)ELF::R_SDIC_NONE;
  unsigned Kind = (unsigned)Fixup.getKind();

  switch (Kind) {
  default:
    llvm_unreachable("invalid fixup kind!");

  case SDIC::fixup_SDIC_CALL16:
    Type = ELF::R_SDIC_CALL16;
    break;
  }
  return Type;
}
//@GetRelocType }


MCObjectWriter *llvm::createSDICELFObjectWriter(raw_pwrite_stream &OS,
                                                uint8_t OSABI,
                                                bool IsLittleEndian) {
  MCELFObjectTargetWriter *MOTW = new SDICELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, IsLittleEndian);
}
