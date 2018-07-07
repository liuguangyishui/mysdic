//===-- SDICAsmBackend.cpp - SDIC Asm Backend  ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the SDICAsmBackend class.
//
//===----------------------------------------------------------------------===//
//
#include "MCTargetDesc/SDICFixupKinds.h"
#include "MCTargetDesc/SDICAsmBackend.h"

#include "MCTargetDesc/SDICMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

//@adjustFixupValue {
// Prepare value for the target space for it
static unsigned adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
				 MCContext *Ctx = nullptr) {
  unsigned Kind = Fixup.getKind();

  //Add/subtract and shift
  switch (Kind) {
  default:
    return 0;
  case SDIC::fixup_SDIC_CALL16:
    break;
  }
  return Value;
}
//@adjustFixupValue }
