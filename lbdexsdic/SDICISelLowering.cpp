//===-- SDICISelLowering.cpp - SDIC DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that SDIC uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//
#include "SDICISelLowering.h"

#include "MCTargetDesc/SDICBaseInfo.h"
#include "SDICMachineFunction.h"
#include "SDICTargetMachine.h"
#include "SDICTargetObjectFile.h"
#include "SDICSubtarget.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "sdic-lower"

//@3_1 1 {
const char *SDICTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case SDICISD::JmpLink:           return "SDICISD::JmpLink";
  case SDICISD::TailCall:          return "SDICISD::TailCall";
  case SDICISD::Hi:                return "SDICISD::Hi";
  case SDICISD::Lo:                return "SDICISD::Lo";
  case SDICISD::GPRel:             return "SDICISD::GPRel";
  case SDICISD::Ret:               return "SDICISD::Ret";
  case SDICISD::EH_RETURN:         return "SDICISD::EH_RETURN";
  case SDICISD::DivRem:            return "SDICISD::DivRem";
  case SDICISD::DivRemU:           return "SDICISD::DivRemU";
  case SDICISD::Wrapper:           return "SDICISD::Wrapper";
  case SDICISD::Movlw:             return "SDICISD::Movlw";
  case SDICISD::Pesuo:             return "SDICISD::Pesuo";
  case SDICISD::Addwf:             return "SDICISD::Addwf";
  case SDICISD::Addtest:           return "SDICISD::Addtest";
  case SDICISD::Return:            return "SDICISD::Return";
  default:                         return NULL;
  }
}
//@3_1 1 }

//@SDICTargetLowering {
SDICTargetLowering::SDICTargetLowering(const SDICTargetMachine &TM,
                                       const SDICSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI), ABI(TM.getABI()) {

  // SDIC Custom Operations

  setOperationAction(ISD::ADD, MVT::i32, Custom);
  //  setOperationAction(ISD::STORE, MVT::i32, Expand);
  //   setOperationAction(ISD::LOAD, MVT::i32, Expand);
  // Operations not directly supported by SDIC.

  //setTargetDAGCombine(ISD::ADD);
  //- Set .align 2
// It will emit .align 2 later
  setMinFunctionAlignment(2);//2

}

const SDICTargetLowering *SDICTargetLowering::create(const SDICTargetMachine &TM,
                                                     const SDICSubtarget &STI) {
  return llvm::createSDICSETargetLowering(TM, STI);
}

//==------------------------------------------------------------------------===//
//                      Custom Lowering Implementation
//===----------------------------------------------------------------------===//

SDValue SDICTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
  printf("5/18 test for add and mov3");
  switch(Op.getOpcode())
    {
    case ISD::ADD:   return LowerADD(Op, DAG);
      //  case ISD::LOAD:
      //   case ISD::STORE: return LowerSTORE(Op, DAG);
      
    default:
      // llvm_unreachable("unimplemented operation")
      ;
    }



}

SDValue SDICTargetLowering::LowerADD(SDValue Op, SelectionDAG &DAG) const
{  printf("5/18 test for add and mov4");
  SDLoc dl(Op);
    printf("5/18 test for add and mov5");
  unsigned Opc = Op.getOpcode();
  SDValue Op0 = Op.getOperand(0);
  SDValue Op1 = Op.getOperand(1);
  EVT VT      = Op.getValueType();
  printf("5/18 test for add and mov6");
  SDValue Flag;
  SDValue Flag0;
  SDValue Flag1;
  
   Flag0 = DAG.getNode(SDICISD::Movlw, dl, VT, Op1);
  
  // return DAG.getNode(SDICISD::Addwf, dl, DAG.getVTList(VT), Op0, Flag0);

  //  Flag0 = DAG.getNode(SDICISD::Addwf, dl, MVT::Other, Op0);
   return DAG.getNode(SDICISD::Addtest, dl,MVT::i32, Op0, Flag0);
   // return DAG.getNode(ISD::ADD, dl, MVT::i32, Op1, Op0);
  
  // Flag1 = DAG.getNode(SDICISD::Addwf, dl, DAG.getVTList(VT), Op0);

  // return DAG.getNode(SDICISD::Pesuo, dl, MVT::i32);// Flag0, Flag1);
  //printf("this is the doc from SDICISelLowering.cpp");
}

SDValue SDICTargetLowering::LowerSTORE(SDValue Op, SelectionDAG &DAG) const
{
  SDLoc dl(Op);

  return DAG.getNode(SDICISD::Pesuo, dl, MVT::Glue);

}



static SDValue perDealwithADD(SDNode *N, SelectionDAG& DAG,
			      TargetLowering::DAGCombinerInfo &DCI,
			      const SDICSubtarget &Subtarget) {

  printf("\n123456hhhhhhhhhhhhhhhhhhhhhhhhhh");

  unsigned Opc = ISD::ADD;

  SDLoc DL(N);

  SDValue ADDwf = DAG.getNode(Opc, DL, MVT::Glue,
			      N->getOperand(0));
  SDValue InChain = DAG.getEntryNode();
  SDValue InGlue = ADDwf;

  
}

SDValue SDICTargetLowering::PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const
{
  SelectionDAG &DAG = DCI.DAG;
  unsigned Opc = N->getOpcode();
  printf("\n this is my test for add\n");
  /*  switch(Opc) {
    default: break;
      case ISD::ADD:
	return perDealwithADD(N, DAG, DCI, Subtarget);
	}*/
  return SDValue();
}



//===----------------------------------------------------------------------===//
//  Lower helper functions
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//

#include "SDICGenCallingConv.inc"

//===----------------------------------------------------------------------===//
//@            Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

//@LowerFormalArguments {
/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.
SDValue
SDICTargetLowering::LowerFormalArguments(SDValue Chain,
                                         CallingConv::ID CallConv,
                                         bool IsVarArg,
                                         const SmallVectorImpl<ISD::InputArg> &Ins,
                                         const SDLoc &DL, SelectionDAG &DAG,
                                         SmallVectorImpl<SDValue> &InVals)
                                          const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  SDICFunctionInfo *SDICFI = MF.getInfo<SDICFunctionInfo>();

  SDICFI->setVarArgsFrameIndex(0);  //{VarArgsFrameIndex= 0} FrameIndex for start of varargs area

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(),
                 ArgLocs, *DAG.getContext());
  SDICCC SDICCCInfo(CallConv, ABI.IsO32(), 
                    CCInfo);
  SDICFI->setFormalArgInfo(CCInfo.getNextStackOffset(),  //first arg==size of incoming argument area
                           SDICCCInfo.hasByValArg()); //True if function has a byval argument //MachineFunction.h

  return Chain;
}
// @LowerFormalArguments }

//===----------------------------------------------------------------------===//
//@              Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue
SDICTargetLowering::LowerReturn(SDValue Chain,
                                CallingConv::ID CallConv, bool IsVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                const SDLoc &DL, SelectionDAG &DAG) const {

  // CCValAssign - represent the assignment of
  // the return value to a location 
  SmallVector<CCValAssign, 16> RVLocs;
  MachineFunction &MF = DAG.getMachineFunction();

// CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs,*DAG.getContext());
  
  SDICCC SDICCCInfo(CallConv, ABI.IsO32(),CCInfo);


  printf("this is for the test of GLUE");
  printf("''''''''''''''''''''''''''''''''''0000000000000");
  // Analyze return values.
  SDICCCInfo.analyzeReturn(Outs, Subtarget.abiUsesSoftFloat(),
			   MF.getFunction()->getReturnType());

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);
 printf("''''''''''''''''''''''''''''''''''1111111111");
  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");
    if (RVLocs[i].getValVT() != RVLocs[i].getLocVT())
      Val = DAG.getNode(ISD::BITCAST, DL, RVLocs[i].getLocVT(), Val);
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Flag);
    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }
  printf("\n'''''''''''''''''''''''''''''''''2222222222");

  //@Ordinary struct type: 2 {
  // The SDIC ABIs for returning structs by value requires that we copy
  // the sret argument into $v0 for the return. We saved the argument into
  // a virtual register in the entry block, so now we copy the value out
  // and into $v0.
  if (MF.getFunction()->hasStructRetAttr()) {
    SDICFunctionInfo *SDICFI = MF.getInfo<SDICFunctionInfo>();
    unsigned Reg = SDICFI->getSRetReturnReg();
    if (!Reg)
      llvm_unreachable("sret virtual register not created in the entry block");
    SDValue Val =
      DAG.getCopyFromReg(Chain, DL, Reg, getPointerTy(DAG.getDataLayout()));
    unsigned V0 = SDIC::R8;
    Chain = DAG.getCopyToReg(Chain, DL, V0, Val, Flag);
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(V0, getPointerTy(DAG.getDataLayout())));
  }
  //@Ordinary struct type: 2 }
 printf("\n'''''''''''''''''''''''''''''''''33333333333");
  RetOps[0] = Chain;
  // Update chain.
  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);
  // Return on SDIC is always a "ret $lr"
  return DAG.getNode(SDICISD::Ret, DL, MVT::Other, RetOps);
}
 

SDICTargetLowering::SDICCC::SDICCC(
  CallingConv::ID CC, bool IsO32_, CCState &Info,
  SDICCC::SpecialCallingConvType SpecialCallingConv_)
  : CCInfo(Info), CallConv(CC), IsO32(IsO32_)
{
  // Pre-allocate reserved argument area.
  CCInfo.AllocateStack(reservedArgArea(), 1);
}

template<typename Ty>
void SDICTargetLowering::SDICCC::
analyzeReturn(const SmallVectorImpl<Ty> &RetVals, bool IsSoftFloat,
	      const SDNode *CallNode, const Type *RetTy) const
{
  CCAssignFn *Fn;
  Fn = RetCC_SDIC;
  for (unsigned I = 0, E = RetVals.size(); I < E; ++I)
    {
    MVT VT = RetVals[I].VT;
    ISD::ArgFlagsTy Flags = RetVals[I].Flags;
    MVT RegVT = this->getRegVT(VT, RetTy, CallNode, IsSoftFloat);
    if (Fn(I, VT, RegVT, CCValAssign::Full, Flags, this->CCInfo))
      {
#ifndef NDEBUG
      dbgs() << "Call result #" << I << " has unhandled type "
	     << EVT(VT).getEVTString() << '\n';
#endif
      llvm_unreachable(nullptr);
      }
    }
}

void SDICTargetLowering::SDICCC::
analyzeCallResult(const SmallVectorImpl<ISD::InputArg> &Ins, bool IsSoftFloat,
		  const SDNode *CallNode, const Type *RetTy) const
{
  analyzeReturn(Ins, IsSoftFloat, CallNode, RetTy);
}

void SDICTargetLowering::SDICCC::
analyzeReturn(const SmallVectorImpl<ISD::OutputArg> &Outs, bool IsSoftFloat,
	      const Type *RetTy) const {
  analyzeReturn(Outs, IsSoftFloat, nullptr, RetTy);
}

unsigned SDICTargetLowering::SDICCC::reservedArgArea() const
{
  return (IsO32 && (CallConv != CallingConv::Fast)) ? 8 : 0;
}


MVT SDICTargetLowering::SDICCC::getRegVT(MVT VT, const Type *OrigTy,
					 const SDNode *CallNode,
					 bool IsSoftFloat) const
{
  if (IsSoftFloat || IsO32)
    return VT;
  return VT;
}

