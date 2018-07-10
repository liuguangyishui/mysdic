//===-- SDICSubtarget.cpp - SDIC Subtarget Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the SDIC specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "SDICSubtarget.h"

#include "SDICMachineFunction.h"
#include "SDIC.h"
#include "SDICRegisterInfo.h"

#include "SDICTargetMachine.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "sdic-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "SDICGenSubtargetInfo.inc"

/*static cl::opt<bool> EnableOverflowOpt
                ("sdic-enable-overflow", cl::Hidden, cl::init(false),
                 cl::desc("Use trigger overflow instructions add and sub \
                 instead of non-overflow instructions addu and subu"));*/

static cl::opt<bool> UseSmallSectionOpt
                ("cpu0-use-small-section", cl::Hidden, cl::init(false),
                 cl::desc("Use small section. Only work when -relocation-model="
                 "static. pic always not use small section."));

static cl::opt<bool> ReserveGPOpt
                ("cpu0-reserve-gp", cl::Hidden, cl::init(false),
                 cl::desc("Never allocate $gp to variable"));

static cl::opt<bool> NoCploadOpt
                ("cpu0-no-cpload", cl::Hidden, cl::init(false),
                 cl::desc("No issue .cpload"));

bool SDICReserveGP;
bool SDICNoCpload;

extern bool FixGlobalBaseReg;

void SDICSubtarget::anchor() { }

//@1 {
SDICSubtarget::SDICSubtarget(const Triple &TT, const std::string &CPU,
                             const std::string &FS, bool little, 
                             const SDICTargetMachine &_TM) :
//@1 }
  // SDICGenSubtargetInfo will display features by llc -march=sdic -mcpu=help
  SDICGenSubtargetInfo(TT, CPU, FS),
  IsLittle(little), TM(_TM), TargetTriple(TT), TSInfo(),
      InstrInfo(
          SDICInstrInfo::create(initializeSubtargetDependencies(CPU, FS, TM))),
      FrameLowering(SDICFrameLowering::create(*this)),
      TLInfo(SDICTargetLowering::create(TM, *this)) {

  // EnableOverflow = EnableOverflowOpt;
   // Set UseSmallSection.
  UseSmallSection = UseSmallSectionOpt;
  SDICReserveGP = ReserveGPOpt;
  SDICNoCpload = NoCploadOpt;

#ifdef ENABLE_GPRESTORE
  if (!TM.isPositionIndependent() && !UseSmallSection && !SDICReserveGP)
    FixGlobalBaseReg = false;
  else
#endif
    FixGlobalBaseReg = true;

}

bool SDICSubtarget::isPositionIndependent() const {
  return TM.isPositionIndependent();
}

SDICSubtarget &
SDICSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                               const TargetMachine &TM) {
  if (TargetTriple.getArch() == Triple::sdic) {
    if (CPU.empty() || CPU == "generic") {
      CPU = "sdic32II";
    }
    else if (CPU == "help") {
      CPU = "";
      return *this;
    }
    else if (CPU != "sdic32I" && CPU != "sdic32II") {
      CPU = "sdic32II";
    }
  }
  else {
    errs() << "!!!Error, TargetTriple.getArch() = " << TargetTriple.getArch()
           <<  "CPU = " << CPU << "\n";
    exit(0);
  }
  
  if (CPU == "sdic32I")
    SDICArchVersion = SDIC32I;
  else if (CPU == "sdic32II")
    SDICArchVersion = SDIC32II;

  if (isSDIC32I()) {
    HasCmp = true;
    HasSlt = false;
  }
  else if (isSDIC32II()) {
    HasCmp = true;
    HasSlt = true;
  }
  else {
    errs() << "-mcpu must be empty(default:sdic32II), sdic32I or sdic32II" << "\n";
  }

  // Parse features string.
  ParseSubtargetFeatures(CPU, FS);
  // Initialize scheduling itinerary for the specified CPU.
  InstrItins = getInstrItineraryForCPU(CPU);

  return *this;
}

bool SDICSubtarget::abiUsesSoftFloat() const {
//  return TM->Options.UseSoftFloat;
  return true;
}

const SDICABIInfo &SDICSubtarget::getABI() const { return TM.getABI(); }

