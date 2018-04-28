//===-- SDICTargetMachine.cpp - Define TargetMachine for SDIC -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about SDIC target spec.
//
//===----------------------------------------------------------------------===//

#include "SDICTargetMachine.h"
#include "SDIC.h"

#include "SDICSEISelDAGToDAG.h"
#include "SDICSubtarget.h"
#include "SDICTargetObjectFile.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

#define DEBUG_TYPE "sdic"

extern "C" void LLVMInitializeSDICTarget() {
  // Register the target.
  //- Big endian Target Machine
  RegisterTargetMachine<SDICebTargetMachine> X(TheSDICTarget);
  //- Little endian Target Machine
  //  RegisterTargetMachine<SDICelTargetMachine> Y(TheSDICelTarget);
}

static std::string computeDataLayout(const Triple &TT, StringRef CPU,
                                     const TargetOptions &Options,
                                     bool isLittle) {
  std::string Ret = "";
  // There are both little and big endian cpu0.
  if (isLittle)
    Ret += "e";
  else
    Ret += "E";

  Ret += "-m:m";

  // Pointers are 32 bit on some ABIs.
  Ret += "-p:32:32";

  // 8 and 16 bit integers only need to have natural alignment, but try to
  // align them to 32 bits. 64 bit integers have natural alignment.
  Ret += "-i8:8:32-i16:16:32";

  // 32 bit registers are always available and the stack is at least 64 bit
  // aligned.
  Ret += "-n32-S64";

  return Ret;
}

static Reloc::Model getEffectiveRelocModel(CodeModel::Model CM,
                                           Optional<Reloc::Model> RM) {
  if (!RM.hasValue() || CM == CodeModel::JITDefault)
    return Reloc::Static;
  return *RM;
}

// DataLayout --> Big-endian, 32-bit pointer/ABI/alignment
// The stack is always 8 byte aligned
// On function prologue, the stack is created by decrementing
// its pointer. Once decremented, all references are done with positive
// offset from the stack/frame pointer, using StackGrowsUp enables
// an easier handling.
// Using CodeModel::Large enables different CALL behavior.
SDICTargetMachine::SDICTargetMachine(const Target &T, const Triple &TT,
                                     StringRef CPU, StringRef FS,
                                     const TargetOptions &Options,
                                     Optional<Reloc::Model> RM,
                                     CodeModel::Model CM, CodeGenOpt::Level OL,
                                     bool isLittle)
  //- Default is big endian
    : LLVMTargetMachine(T, computeDataLayout(TT, CPU, Options, isLittle), TT,
                        CPU, FS, Options, getEffectiveRelocModel(CM, RM), CM,
                        OL),
      isLittle(isLittle), TLOF(make_unique<SDICTargetObjectFile>()),
      ABI(SDICABIInfo::computeTargetABI()),
      DefaultSubtarget(TT, CPU, FS, isLittle, *this) {
  // initAsmInfo will display features by llc -march=cpu0 -mcpu=help on 3.7 but
  // not on 3.6
  initAsmInfo();
}

SDICTargetMachine::~SDICTargetMachine() {}

void SDICebTargetMachine::anchor() { }

SDICebTargetMachine::SDICebTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         CodeModel::Model CM,
                                         CodeGenOpt::Level OL)
    : SDICTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, false) {}

void SDICelTargetMachine::anchor() { }

SDICelTargetMachine::SDICelTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         CodeModel::Model CM,
                                         CodeGenOpt::Level OL)
    : SDICTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, true) {}

const SDICSubtarget *
SDICTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                        ? CPUAttr.getValueAsString().str()
                        : TargetCPU;
  std::string FS = !FSAttr.hasAttribute(Attribute::None)
                       ? FSAttr.getValueAsString().str()
                       : TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = llvm::make_unique<SDICSubtarget>(TargetTriple, CPU, FS, isLittle,
                                         *this);
  }
  return I.get();
}

namespace {
//@SDICPassConfig {
/// SDIC Code Generator Pass Configuration Options.
class SDICPassConfig : public TargetPassConfig {
public:
  SDICPassConfig(SDICTargetMachine *TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  SDICTargetMachine &getSDICTargetMachine() const {
    return getTM<SDICTargetMachine>();
  }

  const SDICSubtarget &getSDICSubtarget() const {
    return *getSDICTargetMachine().getSubtargetImpl();
  }
  bool addInstSelector() override;
};
} // namespace

TargetPassConfig *SDICTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new SDICPassConfig(this, PM);
}

// Install an instruction selector pass using
// the ISelDag to gen SDIC code.
bool SDICPassConfig::addInstSelector() {
  addPass(createSDICSEISelDag(getSDICTargetMachine(), getOptLevel()));
  return false;
}

