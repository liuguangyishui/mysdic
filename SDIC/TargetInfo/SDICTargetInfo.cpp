//===-- SDICTargetInfo.cpp - SDIC Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "SDIC.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheSDICTarget; //llvm::TheSDICelTarget;

extern "C" void LLVMInitializeSDICTargetInfo() {
  RegisterTarget<Triple::sdic,
        /*HasJIT=*/true> X(TheSDICTarget, "sdic", "SDIC");

  //  RegisterTarget<Triple::sdicel,
		 //        /*HasJIT=*/true> Y(TheSDICelTarget, "sdicel", "SDICel");
}

