//===-- SDICSEISelDAGToDAG.cpp - A Dag to Dag Inst Selector for SDICSE ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of SDICDAGToDAGISel specialized for cpu032.
//
//===----------------------------------------------------------------------===//

#include "SDICSEISelDAGToDAG.h"

#include "MCTargetDesc/SDICBaseInfo.h"
#include "SDIC.h"
#include "SDICMachineFunction.h"
#include "SDICRegisterInfo.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
using namespace llvm;

#define DEBUG_TYPE "sdic-isel"

bool SDICSEDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  Subtarget = &static_cast<const SDICSubtarget &>(MF.getSubtarget());
  return SDICDAGToDAGISel::runOnMachineFunction(MF);
}

void SDICSEDAGToDAGISel::processFunctionAfterISel(MachineFunction &MF) {
}

//@selectNode
bool SDICSEDAGToDAGISel::trySelect(SDNode *Node) {
  unsigned Opcode = Node->getOpcode();
  SDLoc DL(Node);

  ///
  // Instruction Selection not handled by the auto-generated
  // tablegen selection should be handled here.
  ///

  ///
  // Instruction Selection not handled by the auto-generated
  // tablegen selection should be handled here.
  ///
  EVT NodeTy = Node->getValueType(0);
  unsigned MultOpc;

  switch(Opcode) {
  default: break;

  case ISD::Constant: {
    const ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Node);
    unsigned Size = CN->getValueSizeInBits(0);
    if(Size == 32)
      break;
    return true;
  }
  }

  return false;
}

FunctionPass *llvm::createSDICSEISelDag(SDICTargetMachine &TM,
                                        CodeGenOpt::Level OptLevel) {
  return new SDICSEDAGToDAGISel(TM, OptLevel);
}

