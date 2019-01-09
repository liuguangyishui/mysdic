//===-- SDICISelDAGToDAG.cpp - A Dag to Dag Inst Selector for SDIC --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the CPU0 target.
//
//===----------------------------------------------------------------------===//

#include "SDICISelDAGToDAG.h"
#include "SDIC.h"
#include "SDICMachineFunction.h"

#include "SDICMachineFunction.h"
#include "SDICRegisterInfo.h"
#include "SDICSEISelDAGToDAG.h"
#include "SDICTargetMachine.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
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

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// SDICDAGToDAGISel - CPU0 specific code to select CPU0 machine
// instructions for SelectionDAG operations.
//===----------------------------------------------------------------------===//

bool SDICDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  bool Ret = SelectionDAGISel::runOnMachineFunction(MF);

  return Ret;
}

/// getGlobalBaseReg - Output the instructions required to put the
/// GOT address into a register.
SDNode *SDICDAGToDAGISel::getGlobalBaseReg() {
  unsigned GlobalBaseReg = MF->getInfo<SDICFunctionInfo>()->getGlobalBaseReg();
  return CurDAG->getRegister(GlobalBaseReg, getTargetLowering()->getPointerTy(
                                                CurDAG->getDataLayout()))
      .getNode();
}


//@SelectAddr {
/// ComplexPattern used on SDICInstrInfo
/// Used on SDIC Load/Store instructions
bool SDICDAGToDAGISel::
SelectAddr(SDNode *Parent, SDValue Addr, SDValue &Base, SDValue &Offset) {
//@SelectAddr }
  EVT ValTy = Addr.getValueType();
  SDLoc DL(Addr);

  // If Parent is an unaligned f32 load or store, select a (base + index)
  // floating point load/store instruction (luxc1 or suxc1).
  const LSBaseSDNode* LS = 0;

  if (Parent && (LS = dyn_cast<LSBaseSDNode>(Parent))) {
    EVT VT = LS->getMemoryVT();

    if (VT.getSizeInBits() / 8 > LS->getAlignment()) {
      assert("Unaligned loads/stores not supported for this type.");
      if (VT == MVT::f32)
        return false;
    }
  }
  
  // if Address is FI, get the TargetFrameIndex.
  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
    Base   = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
    Offset = CurDAG->getTargetConstant(0, DL, ValTy);

    //printf("\nthe SDValue Base is %u",&Base);
    //printf("\nthe SDValue Offser is %u", &Offset);

    return true;
  }


  // on PIC code Load GA
  if (Addr.getOpcode() == SDICISD::Wrapper) {
    Base   = Addr.getOperand(0);
    Offset = Addr.getOperand(1);
    return true;
  }

  //@static
  if (TM.getRelocationModel() != Reloc::PIC_) {
    if ((Addr.getOpcode() == ISD::TargetExternalSymbol ||
        Addr.getOpcode() == ISD::TargetGlobalAddress))
      return false;
  }

   // Addresses of the form FI+const or FI|const
  if (CurDAG->isBaseWithConstantOffset(Addr)) {
    ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1));
    if (isInt<16>(CN->getSExtValue())) {

      // If the first operand is a FI, get the TargetFI Node
      if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>
                                  (Addr.getOperand(0)))
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
      else
        Base = Addr.getOperand(0);

      Offset = CurDAG->getTargetConstant(CN->getZExtValue(), DL, ValTy);
      return true;
    }
  }
  

  
  Base   = Addr;
  Offset = CurDAG->getTargetConstant(0, DL, ValTy);
  return true;
}

//@Select {
/// Select instructions not customized! Used for
/// expanded, promoted and normal instructions
void SDICDAGToDAGISel::Select(SDNode *Node) {
//@Select }
  unsigned Opcode = Node->getOpcode();
  SDLoc DL(Node);
  // Dump information about the Node being selected
  DEBUG(errs() << "Selecting: "; Node->dump(CurDAG); errs() << "\n");

  // If we have a custom node, we already have selected!
  if (Node->isMachineOpcode()) {
    DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    Node->setNodeId(-1);
    return;
  }
  //HYL  SDLoc DL(Node);
  // See if subclasses can handle this node.
  if (trySelect(Node))
    return;

  switch(Opcode) {
  default: break;

    /*case ISD::ADD: {
    SDValue op1 = Node->getOperand(0);
    SDValue op2 = Node->getOperand(1);
    
    //SDNode* newNode = CurDAG->getMachineNode(SDICISD::Movf, DL, MVT::i32, op1);
    //CurDAG->SelectNodeTo(Node, SDICISD::Addwf
     //HYL   ReplaceNode(Node, CurDAG->getMachineNode(SDICISD::Addwf, DL, MVT::i32));
     // Get target GOT address.
  }
  case ISD::STORE: {
    /* StoreSDNode *ST = cast<StoreSDNode>(Node);
    EVT StoreVT = ST->getMemoryVT();
    
    SDValue Chain = Node->getOperand(0);
    SDValue op1 = Node->getOperand(1);
    SDValue op2 = Node->getOperand(2);
   
    SDValue Ops[] = {op1, op2, Chain};

    SDNode *NewNode = CurDAG->getMachineNode(SDICISD::Movf, DL, MVT::i32, op1);
    // ReplaceNode(Node, NewNode);
    ReplaceUses(SDValue(Node, 0), SDValue(NewNode, 1));
    //   CurDAG->SelectNodeTo(Node, SDICISD::Addwf, MVT::i32);
    }*/
 
    /*    case ISD::LOAD: {
      SDValue Chain = Node->getOperand(0);
      SDValue op1 = Node->getOperand(0);
      SDValue op2 = Node->getOperand(1);
      SDValue op3 = Node->getOperand(2);
      //      SDValue valueType1 = op1->getValueType();
      // SDValue valueType2 = op2->getValueType();
      //printf("valueType1 %u\n", valueType1);
      //printf("valueType2 %u\n", valueType2);
 
      //      SDNode *NewNode = CurDAG->getMachineNode(SDICISD::Addwf, DL, MVT::i32, MVT::Other, op1, op2);

      SDNode *NewNode = CurDAG->getMachineNode(SDIC::TestNode, DL, MVT::i32,  MVT::Other, op1, op2);//, op3);
      ReplaceNode(Node, NewNode);
      //      CurDAG->RemoveDeadNode(Node);
      }*/
    case ISD::GLOBAL_OFFSET_TABLE:
      ReplaceNode(Node, getGlobalBaseReg());
      return;

    
  }

  // Select the default instruction
  SelectCode(Node);
}

