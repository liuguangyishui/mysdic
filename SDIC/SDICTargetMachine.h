//===-- SDICTargetMachine.h - Define TargetMachine for SDIC -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the SDIC specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDICTARGETMACHINE_H
#define LLVM_LIB_TARGET_SDIC_SDICTARGETMACHINE_H

#include "SDICConfig.h"

#include "MCTargetDesc/SDICABIInfo.h"
#include "SDICSubtarget.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class formatted_raw_ostream;
class SDICRegisterInfo;

class SDICTargetMachine : public LLVMTargetMachine {
  bool isLittle;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  // Selected ABI
  SDICABIInfo ABI;
  SDICSubtarget DefaultSubtarget;

  mutable StringMap<std::unique_ptr<SDICSubtarget>> SubtargetMap;
public:
  SDICTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                    StringRef FS, const TargetOptions &Options,
                    Optional<Reloc::Model> RM, CodeModel::Model CM,
                    CodeGenOpt::Level OL, bool isLittle);
  ~SDICTargetMachine() override;

  const SDICSubtarget *getSubtargetImpl() const {
    return &DefaultSubtarget;
  }

  const SDICSubtarget *getSubtargetImpl(const Function &F) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
  bool isLittleEndian() const { return isLittle; }
  const SDICABIInfo &getABI() const { return ABI; }
};

/// SDICebTargetMachine - SDIC32 big endian target machine.
///
class SDICebTargetMachine : public SDICTargetMachine {
  virtual void anchor();
public:
  SDICebTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      Optional<Reloc::Model> RM, CodeModel::Model CM,
                      CodeGenOpt::Level OL);
};

/// SDICelTargetMachine - SDIC32 little endian target machine.
///
class SDICelTargetMachine : public SDICTargetMachine {
  virtual void anchor();
public:
  SDICelTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      Optional<Reloc::Model> RM, CodeModel::Model CM,
                      CodeGenOpt::Level OL);
};
} // End llvm namespace

#endif

