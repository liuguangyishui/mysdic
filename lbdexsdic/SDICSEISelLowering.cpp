//===-- SDICSEISelLowering.cpp - SDICSE DAG Lowering Interface --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of SDICTargetLowering specialized for sdic32.
//
//===----------------------------------------------------------------------===//
#include "SDICMachineFunction.h"
#include "SDICSEISelLowering.h"

#include "SDICRegisterInfo.h"
#include "SDICTargetMachine.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetInstrInfo.h"

using namespace llvm;

#define DEBUG_TYPE "sdic-isel"

static cl::opt<bool>
EnableSDICTailCalls("enable-sdic-tail-calls", cl::Hidden,
                    cl::desc("SDIC: Enable tail calls."), cl::init(false));

//@SDICSETargetLowering {
SDICSETargetLowering::SDICSETargetLowering(const SDICTargetMachine &TM,
                                           const SDICSubtarget &STI)
    : SDICTargetLowering(TM, STI) {
//@SDICSETargetLowering body {
  // Set up the register classes
  addRegisterClass(MVT::i32, &SDIC::SDICRegsRegClass);

// must, computeRegisterProperties - Once all of the register classes are 
//  added, this allows us to compute derived properties we expose.
  computeRegisterProperties(Subtarget.getRegisterInfo());
}

SDValue SDICSETargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {

  return SDICTargetLowering::LowerOperation(Op, DAG);
}

const SDICTargetLowering *
llvm::createSDICSETargetLowering(const SDICTargetMachine &TM,
                                 const SDICSubtarget &STI) {
  return new SDICSETargetLowering(TM, STI);
}

bool SDICSETargetLowering::
isEligibleForTailCallOptimization(const SDICCC &SDICCCInfo,
                                  unsigned NextStackOffset,
                                  const SDICFunctionInfo& FI) const {
  if (!EnableSDICTailCalls)
    return false;

  // Return false if either the callee or caller has a byval argument.
  if (SDICCCInfo.hasByValArg() || FI.hasByvalArg())
    return false;

  // Return true if the callee's argument area is no larger than the
  // caller's.
  return NextStackOffset <= FI.getIncomingArgSize();
}
