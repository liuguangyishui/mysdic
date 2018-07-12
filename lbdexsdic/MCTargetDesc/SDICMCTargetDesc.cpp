//===-- SDICMCTargetDesc.cpp - SDIC Target Descriptions -------------------===//
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

#include "SDICMCTargetDesc.h"
#include "InstPrinter/SDICInstPrinter.h"
#include "SDICMCAsmInfo.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "SDICGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "SDICGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "SDICGenRegisterInfo.inc"

//@1 {
/// Select the SDIC Architecture Feature for the given triple and cpu name.
/// The function will be called at command 'llvm-objdump -d' for SDIC elf input.
static StringRef selectSDICArchFeature(const Triple &TT, StringRef CPU) {
  std::string SDICArchFeature;
  if (CPU.empty() || CPU == "generic") {
    if (TT.getArch() == Triple::sdic) {
      if (CPU.empty() || CPU == "sdic32II") {
        SDICArchFeature = "+sdic32II";
      }
      else {
        if (CPU == "sdic32I") {
          SDICArchFeature = "+sdic32I";
        }
      }
    }
  }
  return SDICArchFeature;
}
//@1 }

static MCInstrInfo *createSDICMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitSDICMCInstrInfo(X); // defined in SDICGenInstrInfo.inc
  return X;
}

static MCRegisterInfo *createSDICMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitSDICMCRegisterInfo(X, SDIC::STATUS); // defined in SDICGenRegisterInfo.inc
  return X;
}

static MCSubtargetInfo *createSDICMCSubtargetInfo(const Triple &TT,
                                                  StringRef CPU, StringRef FS) {
  std::string ArchFS = selectSDICArchFeature(TT,CPU);
  if (!FS.empty()) {
    if (!ArchFS.empty())
      ArchFS = ArchFS + "," + FS.str();
    else
      ArchFS = FS;
  }
  return createSDICMCSubtargetInfoImpl(TT, CPU, ArchFS);
// createSDICMCSubtargetInfoImpl defined in SDICGenSubtargetInfo.inc
}

static MCAsmInfo *createSDICMCAsmInfo(const MCRegisterInfo &MRI,
                                      const Triple &TT) {
  MCAsmInfo *MAI = new SDICMCAsmInfo(TT);

  unsigned SP = MRI.getDwarfRegNum(SDIC::R10, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(nullptr, SDIC::TOSL, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCInstPrinter *createSDICMCInstPrinter(const Triple &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI) {
  return new SDICInstPrinter(MAI, MII, MRI);
}

namespace {

class SDICMCInstrAnalysis : public MCInstrAnalysis {
public:
  SDICMCInstrAnalysis(const MCInstrInfo *Info) : MCInstrAnalysis(Info) {}
};
}

static MCInstrAnalysis *createSDICMCInstrAnalysis(const MCInstrInfo *Info) {
  return new SDICMCInstrAnalysis(Info);
}

//@2 {
extern "C" void LLVMInitializeSDICTargetMC() {
  for (Target *T :{ &TheSDICTarget}) {
    // Register the MC asm info.
    RegisterMCAsmInfoFn X(*T, createSDICMCAsmInfo);

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createSDICMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createSDICMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T,
	                                        createSDICMCSubtargetInfo);
    // Register the MC instruction analyzer.
    TargetRegistry::RegisterMCInstrAnalysis(*T, createSDICMCInstrAnalysis);
    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(*T,
	                                      createSDICMCInstPrinter);
  }

}
//@2 }

