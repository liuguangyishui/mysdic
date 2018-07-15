//===-- SDICSubtarget.h - Define Subtarget for the SDIC ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the SDIC specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDICSUBTARGET_H
#define LLVM_LIB_TARGET_SDIC_SDICSUBTARGET_H

#include "SDICConfig.h"

#include "SDICFrameLowering.h"
#include "SDICISelLowering.h"
#include "SDICInstrInfo.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/Target/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER
#include "SDICGenSubtargetInfo.inc"

extern bool SDICReserveGP;
extern bool SDICNoCpload;

//@1
namespace llvm {
class StringRef;

class SDICTargetMachine;

class SDICSubtarget : public SDICGenSubtargetInfo {
  virtual void anchor();

public:

  bool HasChapterDummy;
  bool HasChapterAll;

  bool hasChapter3_1() const {
    return true;
  }

  bool hasChapter3_2() const {
    return true;
  }

  bool hasChapter3_3() const {
    return true;
  }

  bool hasChapter3_4() const {
    return true;
    //return false;
  }

  bool hasChapter3_5() const {
    return true;
    //return false;
  }

  bool hasChapter4_1() const {
    return true;
    //    return false;
  }

  bool hasChapter4_2() const {
    return true;
    //    return false;
  }

  bool hasChapter5_1() const {
     return true;
     // return false;
  }

  bool hasChapter6_1() const {
     return true;
     //return false;
  }

  bool hasChapter7_1() const {
    return false;
  }

  bool hasChapter8_1() const {
    return false;
  }

  bool hasChapter8_2() const {
    return false;
  }
  
  bool hasChapter9_1() const {
    return false;
  }

  bool hasChapter9_2() const {
    return false;
  }

  bool hasChapter9_3() const {
    return false;
  }

  bool hasChapter10_1() const {
    return false;
  }

  bool hasChapter11_1() const {
    return false;
  }

  bool hasChapter11_2() const {
    return false;
  }

  bool hasChapter12_1() const {
    return false;
  }

protected:
  enum SDICArchEnum {
    SDIC32I,
    SDIC32II
  };

  // SDIC architecture version
  SDICArchEnum SDICArchVersion;

  // IsLittle - The target is Little Endian
  bool IsLittle;

  bool EnableOverflow;

  // HasCmp - cmp instructions.
  bool HasCmp;

  // HasSlt - slt instructions.
  bool HasSlt;

  InstrItineraryData InstrItins;

   // UseSmallSection - Small section is used.
  bool UseSmallSection;

  const SDICTargetMachine &TM;

  Triple TargetTriple;

  const SelectionDAGTargetInfo TSInfo;

  std::unique_ptr<const SDICInstrInfo> InstrInfo;
  std::unique_ptr<const SDICFrameLowering> FrameLowering;
  std::unique_ptr<const SDICTargetLowering> TLInfo;

public:
  bool isPositionIndependent() const;
  const SDICABIInfo &getABI() const;

  /// This constructor initializes the data members to match that
  /// of the specified triple.
  SDICSubtarget(const Triple &TT, const std::string &CPU, const std::string &FS,
                bool little, const SDICTargetMachine &_TM);

//- Vitual function, must have
  /// ParseSubtargetFeatures - Parses features string setting specified
  /// subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  bool isLittle() const { return IsLittle; }
  bool hasSDIC32I() const { return SDICArchVersion >= SDIC32I; }
  bool isSDIC32I() const { return SDICArchVersion == SDIC32I; }
  bool hasSDIC32II() const { return SDICArchVersion >= SDIC32II; }
  bool isSDIC32II() const { return SDICArchVersion == SDIC32II; }

  /// Features related to the presence of specific instructions.
  bool enableOverflow() const { return EnableOverflow; }
  bool disableOverflow() const { return !EnableOverflow; }
  bool hasCmp()   const { return HasCmp; }
  bool hasSlt()   const { return HasSlt; }

  bool useSmallSection() const { return UseSmallSection; }

  bool abiUsesSoftFloat() const;

  bool enableLongBranchPass() const {
    return hasSDIC32II();
  }
  
  unsigned stackAlignment() const { return 8; }

  SDICSubtarget &initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                                 const TargetMachine &TM);

  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }
  const SDICInstrInfo *getInstrInfo() const override { return InstrInfo.get(); }
  const TargetFrameLowering *getFrameLowering() const override {
    return FrameLowering.get();
  }
  const SDICRegisterInfo *getRegisterInfo() const override {
    return &InstrInfo->getRegisterInfo();
  }
  const SDICTargetLowering *getTargetLowering() const override {
    return TLInfo.get();
  }
  const InstrItineraryData *getInstrItineraryData() const override {
    return &InstrItins;
  }
};
} // End llvm namespace

#endif // #if CH >= CH3_1

