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
#include "llvm/CodeGen/SelectionDAGNodes.h"

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
  case SDICISD::Movf:              return "SDICISD::Movf";
  case SDICISD::Pesuo:             return "SDICISD::Pesuo";
  case SDICISD::Pesuo_NoRet:       return "SDICISD::Pesuo_NoRet";
  case SDICISD::Pesuo_None:        return "SDICISD::Pesuo_None";
  case SDICISD::Addwf:             return "SDICISD::Addwf";
  case SDICISD::Addwfrr:           return "SDICISD::Addwfrr";
  case SDICISD::Mul_Call:          return "SDICISD::Mul_Call";
  case SDICISD::Div_Call:          return "SDICISD::Div_Call";
  case SDICISD::Subwf:             return "SDICISD::Subwf";
  case SDICISD::Addtest:           return "SDICISD::Addtest";
  case SDICISD::GPReltest:         return "SDICISD::GPReltest";
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
  setOperationAction(ISD::MUL, MVT::i32, Custom);
  setOperationAction(ISD::SDIV, MVT::i32, Custom);
  setOperationAction(ISD::SUB, MVT::i32, Custom);



  
  // setOperationAction(ISD::STORE, MVT::i32, Custom);
   

   
  //setOperationAction(ISD::LOAD, MVT::i32, Custom);

   
  //   setOperationAction(ISD::LOAD, MVT::i32, Expand);
  // Operations not directly supported by SDIC.

  setOperationAction(ISD::BR_CC,             MVT::i32,  Expand);


  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1 , Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8 , Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16 , Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i32 , Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::Other , Expand);
  


  
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
    case ISD::MUL:   return LowerMUL(Op, DAG);
    case ISD::SDIV:   return LowerDIV(Op, DAG);

    case ISD::SUB:   return LowerSUB(Op, DAG);
      // case ISD::LOAD:  return LowerLOAD(Op, DAG);

      
      //  case ISD::STORE: return LowerSTORE(Op, DAG);
    
      
    default:
      // llvm_unreachable("unimplemented operation")
      ;
    }



}

SDValue SDICTargetLowering::LowerADD(SDValue Op, SelectionDAG &DAG) const
{
   
  printf("5/18 test for add and mov4");
  SDLoc dl(Op);
    printf("5/18 test for add and mov5");
  unsigned Opc = Op.getOpcode();
  //Op0一直都是寄存器
  SDValue Op0 = Op.getOperand(0);//This is register number
  //Op1则是立即数或寄存器
  SDValue Op1 = Op.getOperand(1);//This is a constant
  EVT VT      = Op.getValueType();
  printf("5/18 test for add and mov6");
  SDValue Flag;
  SDValue Flag0;
  SDValue Flag1;

  //ADD加法有两种情况，一个是ADDLW：WREG与立即数K相加；  一个是ADDWF与F寄存器相加

  
  //Flag0 = DAG.getNode(SDICISD::Movf, dl, MVT::i32, Op0);
 
  return DAG.getNode(SDICISD::Addwf, dl,MVT::i32, Op1, Op0);
 
 
   // Flag0 = DAG.getConstant(45, dl, MVT::i32);

   //  Flag0 = DAG.getNode(ISD::ADD, dl, DAG.getVTList(MVT::i32),Op0, Op1);
  
  //  return Flag0;

  //  Flag0 = DAG.getNode(SDICISD::Addwf, dl, MVT::Other, Op0);
  //---- return DAG.getNode(SDICISD::Addtest, dl,MVT::i32, Op0, Flag0);

}
SDValue SDICTargetLowering::LowerMUL(SDValue Op, SelectionDAG &DAG) const
{
  SDLoc dl(Op);
  SDValue Op0 = Op.getOperand(0);
  SDValue Op1 = Op.getOperand(1);//this is a constant
  EVT VT      = Op.getValueType();

  SDValue Flag0;
  SDValue Flag1;

  SDValue Opa = DAG.getConstant(0x00, dl, MVT::i32);
  SDValue Flaga = DAG.getNode(SDICISD::Movlw, dl, MVT::i32, Opa);
  


  
  Flag1 = DAG.getNode(SDICISD::Movlw, dl, MVT::i32, Op1);
  Flag0 = DAG.getNode(SDICISD::Movlw, dl, MVT::i32, Op0);
  return  DAG.getNode(SDICISD::Mul_Call, dl, MVT::i32, Flag0, Flag1);
}

SDValue SDICTargetLowering::LowerDIV(SDValue Op, SelectionDAG &DAG) const
{
  SDLoc dl(Op);
  SDValue Op0 = Op.getOperand(0);
  SDValue Op1 = Op.getOperand(1);

  SDValue Flag0;
  SDValue Flag1;
  Flag0 = DAG.getNode(SDICISD::Movlw, dl, MVT::i32, Op1);
  Flag1 = DAG.getNode(SDICISD::Movlw, dl, MVT::i32, Op0);
  return DAG.getNode(SDICISD::Div_Call, dl, MVT::i32, Flag0, Flag1);

}


SDValue SDICTargetLowering::LowerSUB(SDValue Op, SelectionDAG &DAG) const
{
  SDLoc dl(Op);
  SDValue Op0 = Op.getOperand(0);
  SDValue Op1 = Op.getOperand(1);
  EVT VT      = Op.getValueType();
  SDValue Flag0;
  //Flag0 = DAG.getNode(SDICISD::Movlw, dl, MVT::i32, Op1);
  return DAG.getNode(SDICISD::Subwf, dl, MVT::i32, Op0, Op1);
}



SDValue SDICTargetLowering::LowerSTORE(SDValue Op, SelectionDAG &DAG) const
{
  SDLoc dl(Op);

  unsigned numvlaues = Op->getNumValues();
  printf("\nThe numvalue is %u\n",numvlaues);
  int i = 0;
  while(i<=0)
    {
      SDValue value = Op.getOperand(i);
      printf("The %d values if %u\n",i,value);
      i = i+1;
    }
  // SDValue Op0 = Op.getOperand(0);
    SDValue Op1 = Op.getOperand(1);
     SDValue Op0 = Op.getOperand(0);

    //   SDValue Op2 = Op.getOperand(2);
    //  SDValue Op3 = Op.getOperand(3);
  printf("this is a test");


   MachineFunction &MF = DAG.getMachineFunction();

    if(MF.getFunction()->getReturnType()->isVoidTy())
    {
      printf("\n this is voidTy");
       return DAG.getNode(SDICISD::Pesuo_NoRet, dl, MVT::Other, Op1);
    }
   else
    {
      printf("\n this is not voidTy");
      //   SDValue Flag0 = DAG.getNode(SDICISD::Pesuo, dl, MVT::i32, Op0);//R
       //return DAG.getNode(SDICISD::Pesuo, dl, MVT::Other, Op1);//, Op2, Op3);
      //SDValue Flag0 = DAG.getNode(ISD::EntryToken, dl, MVT::i32);
      //return DAG.getNode(SDICISD::Addwfrr, dl, MVT::Other, Op1, Op0);//,  Flag0);//C + R
   
    }
   
}

SDValue SDICTargetLowering::LowerLOAD(SDValue Op, SelectionDAG &DAG) const
{
  SDLoc dl(Op);

  LoadSDNode *LD = cast<LoadSDNode>(Op);
  SDValue Chain = LD->getChain();
  SDValue BasePtr = LD->getBasePtr();

  switch (BasePtr.getOpcode())
    {
    default: printf("this is defualt");
      break;
      //  case ISD::FrameIndex:
      //   printf("\n this is FrameIndex");
      //  return SDValue();
    }

  
  SDValue Op0 = Op.getOperand(0);
  SDValue Op1 = Op.getOperand(1);
  SDValue Op2 = Op.getOperand(2);
  SDValue Op3 = Op.getOperand(3);

  SDValue Lo = DAG.getLoad(MVT::i8, dl, Chain, BasePtr,
			   MachinePointerInfo());//, LD->isVolatile(), LD->isNonTemporal(),
  //			   LD->isInvariant(), LD->getAlignment());

  SDValue HighAddr = DAG.getNode(ISD::ADD, dl, MVT::i16, BasePtr,
				 DAG.getConstant(1, dl, MVT::i16));

   SDValue Hi = DAG.getLoad(MVT::i8, dl, Chain, HighAddr,
			    MachinePointerInfo());//, LD->isVolatile(), LD->isNonTemporal(),
   //			    LD->isInvariant(), LD->getAlignment());

  // return DAG.getNode(ISD::LOAD, dl, MVT::Other, Op0, Op1, Op2, Op3);
  //return DAG.getNode(SDICISD::Movlw, dl, MVT::i32, Op0);
   return DAG.getNode(ISD::TokenFactor, dl, MVT::Other, Lo.getValue(1), Hi.getValue(1));

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
// addLiveIn - This helper function adds the specified physical register to the
// MachineFunction as a live in value.  It also creates a corresponding
// virtual register for it.
static unsigned
addLiveIn(MachineFunction &MF, unsigned PReg, const TargetRegisterClass *RC)
{
  unsigned VReg = MF.getRegInfo().createVirtualRegister(RC);
  MF.getRegInfo().addLiveIn(PReg, VReg);
  return VReg;
}
//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//
// Passed in stack only.
static bool CC_SDICS32(unsigned ValNo, MVT ValVT, MVT LocVT,
                       CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                       CCState &State) {
  // Do not process byval args here.
  if (ArgFlags.isByVal())
    return true;

  // Promote i8 and i16
  if (LocVT == MVT::i8 || LocVT == MVT::i16) {
    LocVT = MVT::i32;
    if (ArgFlags.isSExt())
      LocInfo = CCValAssign::SExt;
    else if (ArgFlags.isZExt())
      LocInfo = CCValAssign::ZExt;
    else
      LocInfo = CCValAssign::AExt;
  }

  unsigned OrigAlign = ArgFlags.getOrigAlign();
  unsigned Offset = State.AllocateStack(ValVT.getSizeInBits() >> 3,
                                        OrigAlign);
  State.addLoc(CCValAssign::getMem(ValNo, ValVT, Offset, LocVT, LocInfo));
  return false;
}


// Passed first two i32 arguments in registers and others in stack.
static bool CC_SDICO32(unsigned ValNo, MVT ValVT, MVT LocVT,
                       CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                       CCState &State) {
  static const MCPhysReg IntRegs[] = { SDIC::R0, SDIC::R1 };

  // Do not process byval args here.
  if (ArgFlags.isByVal())
    return true;

  // Promote i8 and i16
  if (LocVT == MVT::i8 || LocVT == MVT::i16) {
    LocVT = MVT::i32;
    if (ArgFlags.isSExt())
      LocInfo = CCValAssign::SExt;
    else if (ArgFlags.isZExt())
      LocInfo = CCValAssign::ZExt;
    else
      LocInfo = CCValAssign::AExt;
  }

  unsigned Reg;

  // f32 and f64 are allocated in A0, A1 when either of the following
  // is true: function is vararg, argument is 3rd or higher, there is previous
  // argument which is not f32 or f64.
  bool AllocateFloatsInIntReg = true;
  unsigned OrigAlign = ArgFlags.getOrigAlign();
  bool isI64 = (ValVT == MVT::i32 && OrigAlign == 8);

  if (ValVT == MVT::i32 || (ValVT == MVT::f32 && AllocateFloatsInIntReg)) {
    Reg = State.AllocateReg(IntRegs);
    // If this is the first part of an i64 arg,
    // the allocated register must be A0.
    if (isI64 && (Reg == SDIC::R1))
      Reg = State.AllocateReg(IntRegs);
    LocVT = MVT::i32;
  } else if (ValVT == MVT::f64 && AllocateFloatsInIntReg) {
    // Allocate int register. If first
    // available register is SDIC::A1, shadow it too.
    Reg = State.AllocateReg(IntRegs);
    if (Reg == SDIC::R1)
      Reg = State.AllocateReg(IntRegs);
    State.AllocateReg(IntRegs);
    LocVT = MVT::i32;
  } else
    llvm_unreachable("Cannot handle this ValVT.");

  if (!Reg) {
    unsigned Offset = State.AllocateStack(ValVT.getSizeInBits() >> 3,
                                          OrigAlign);
    State.addLoc(CCValAssign::getMem(ValNo, ValVT, Offset, LocVT, LocInfo));
  } else
    State.addLoc(CCValAssign::getReg(ValNo, ValVT, Reg, LocVT, LocInfo));

  return false;
}



#include "SDICGenCallingConv.inc"

//===----------------------------------------------------------------------===//
//                  Call Calling Convention Implementation
//===----------------------------------------------------------------------===//

static const MCPhysReg O32IntRegs[] = {
  SDIC::R0, SDIC::R1
  // SDIC::R2, SDIC::R3
  // SDIC::R4, SDIC::R5
};

SDValue
SDICTargetLowering::passArgOnStack(SDValue StackPtr, unsigned Offset,
                                   SDValue Chain, SDValue Arg, const SDLoc &DL,
                                   bool IsTailCall, SelectionDAG &DAG) const {
  if (!IsTailCall) {
    SDValue PtrOff =
        DAG.getNode(ISD::ADD, DL, getPointerTy(DAG.getDataLayout()), StackPtr,
                    DAG.getIntPtrConstant(Offset, DL));
    return DAG.getStore(Chain, DL, Arg, PtrOff, MachinePointerInfo());
  }

  MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();
  int FI = MFI->CreateFixedObject(Arg.getValueSizeInBits() / 8, Offset, false);
  SDValue FIN = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
  return DAG.getStore(Chain, DL, Arg, FIN, MachinePointerInfo(),
                      /* Alignment = */ 0, MachineMemOperand::MOVolatile);
}


void SDICTargetLowering::
getOpndList(SmallVectorImpl<SDValue> &Ops,
            std::deque< std::pair<unsigned, SDValue> > &RegsToPass,
            bool IsPICCall, bool GlobalOrExternal, bool InternalLinkage,
            CallLoweringInfo &CLI, SDValue Callee, SDValue Chain) const {
  // T9 should contain the address of the callee function if
  // -reloction-model=pic or it is an indirect call.
  if (IsPICCall || !GlobalOrExternal) {
    unsigned T9Reg = SDIC::R9;
    RegsToPass.push_front(std::make_pair(T9Reg, Callee));
  } else
    Ops.push_back(Callee);

  // Insert node "GP copy globalreg" before call to function.
  //
  // R_CPU0_CALL* operators (emitted when non-internal functions are called
  // in PIC mode) allow symbols to be resolved via lazy binding.
  // The lazy binding stub requires GP to point to the GOT.
  if (IsPICCall && !InternalLinkage) {
    unsigned GPReg = SDIC::R10;
    EVT Ty = MVT::i32;
    RegsToPass.push_back(std::make_pair(GPReg, getGlobalReg(CLI.DAG, Ty)));
  }

  // Build a sequence of copy-to-reg nodes chained together with token
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag in necessary since all emitted instructions must be
  // stuck together.
  SDValue InFlag;

  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = CLI.DAG.getCopyToReg(Chain, CLI.DL, RegsToPass[i].first,
                                 RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
    Ops.push_back(CLI.DAG.getRegister(RegsToPass[i].first,
                                      RegsToPass[i].second.getValueType()));

  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *Mask = 
      TRI->getCallPreservedMask(CLI.DAG.getMachineFunction(), CLI.CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(CLI.DAG.getRegisterMask(Mask));

  if (InFlag.getNode())
    Ops.push_back(InFlag);
}




//@LowerCall {
/// LowerCall - functions arguments are copied from virtual regs to
/// (physical regs)/(stack frame), CALLSEQ_START and CALLSEQ_END are emitted.
SDValue
SDICTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                              SmallVectorImpl<SDValue> &InVals) const {

  SelectionDAG &DAG                     = CLI.DAG;
  SDLoc DL                              = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals     = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins   = CLI.Ins;
  SDValue Chain                         = CLI.Chain;
  SDValue Callee                        = CLI.Callee;
  bool &IsTailCall                      = CLI.IsTailCall;
  CallingConv::ID CallConv              = CLI.CallConv;
  bool IsVarArg                         = CLI.IsVarArg;

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  const TargetFrameLowering *TFL = MF.getSubtarget().getFrameLowering();
  SDICFunctionInfo *FuncInfo = MF.getInfo<SDICFunctionInfo>();
  bool IsPIC = isPositionIndependent();
  SDICFunctionInfo *SDICFI = MF.getInfo<SDICFunctionInfo>();

 // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(),
                 ArgLocs, *DAG.getContext());
  SDICCC::SpecialCallingConvType SpecialCallingConv =
    getSpecialCallingConv(Callee);
  SDICCC SDICCCInfo(CallConv, ABI.IsO32(), 
                    CCInfo, SpecialCallingConv);

  SDICCCInfo.analyzeCallOperands(Outs, IsVarArg,
                                 Subtarget.abiUsesSoftFloat(),
                                 Callee.getNode(), CLI.getArgs());

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NextStackOffset = CCInfo.getNextStackOffset();

  //@TailCall 1 {
  // Check if it's really possible to do a tail call.
  if (IsTailCall)
    IsTailCall =
      isEligibleForTailCallOptimization(SDICCCInfo, NextStackOffset,
                                        *MF.getInfo<SDICFunctionInfo>());

  if (!IsTailCall && CLI.CS && CLI.CS->isMustTailCall())
    report_fatal_error("failed to perform tail call elimination on a call "
                       "site marked musttail");

  if (IsTailCall)
    ++NumTailCalls;
  //@TailCall 1 }

  // Chain is the output chain of the last Load/Store or CopyToReg node.
  // ByValChain is the output chain of the last Memcpy node created for copying
  // byval arguments to the stack.
  unsigned StackAlignment = TFL->getStackAlignment();
  NextStackOffset = alignTo(NextStackOffset, StackAlignment);
  SDValue NextStackOffsetVal = DAG.getIntPtrConstant(NextStackOffset, DL, true);

  //@TailCall 2 {
  if (!IsTailCall)
    Chain = DAG.getCALLSEQ_START(Chain, NextStackOffsetVal, DL);
  //@TailCall 2 }

  SDValue StackPtr =
      DAG.getCopyFromReg(Chain, DL, SDIC::R10,
                         getPointerTy(DAG.getDataLayout()));

  // With EABI is it possible to have 16 args on registers.
  std::deque< std::pair<unsigned, SDValue> > RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDICCC::byval_iterator ByValArg = SDICCCInfo.byval_begin();

  // With EABI is it possible to have 16 args on registers.
  std::deque< std::pair<unsigned, SDValue> > RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDICCC::byval_iterator ByValArg = SDICCCInfo.byval_begin();

  //@1 {
  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
  //@1 }
    SDValue Arg = OutVals[i];
    CCValAssign &VA = ArgLocs[i];
    MVT LocVT = VA.getLocVT();
    ISD::ArgFlagsTy Flags = Outs[i].Flags;

   //@ByVal Arg {
    if (Flags.isByVal()) {
      assert(Flags.getByValSize() &&
             "ByVal args of size 0 should have been ignored by front-end.");
      assert(ByValArg != SDICCCInfo.byval_end());
      assert(!IsTailCall &&
             "Do not tail-call optimize if there is a byval argument.");
      passByValArg(Chain, DL, RegsToPass, MemOpChains, StackPtr, MFI, DAG, Arg,
                   SDICCCInfo, *ByValArg, Flags, Subtarget.isLittle());
      ++ByValArg;
      continue;
    }
    //@ByVal Arg }

     // Promote the value if needed.
    switch (VA.getLocInfo()) {
    default: llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full:
      break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, DL, LocVT, Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, DL, LocVT, Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, DL, LocVT, Arg);
      break;
    }

     // Arguments that can be passed on register must be kept at
    // RegsToPass vector
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
      continue;
    }

    // Register can't get to this point...
    assert(VA.isMemLoc());

    // emit ISD::STORE whichs stores the
    // parameter value to a stack Location
    MemOpChains.push_back(passArgOnStack(StackPtr, VA.getLocMemOffset(),
                                         Chain, Arg, DL, IsTailCall, DAG));
  }

  // Transform all store nodes into one single node because all store
  // nodes are independent of each other.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

   // If the callee is a GlobalAddress/ExternalSymbol node (quite common, every
  // direct call is) turn it into a TargetGlobalAddress/TargetExternalSymbol
  // node so that legalize doesn't hack it.
  bool IsPICCall = IsPIC; // true if calls are translated to
                                         // jalr $t9
  bool GlobalOrExternal = false, InternalLinkage = false;
  SDValue CalleeLo;
  EVT Ty = Callee.getValueType();

  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    if (IsPICCall) {
      const GlobalValue *Val = G->getGlobal();
      InternalLinkage = Val->hasInternalLinkage();

      if (InternalLinkage)
        Callee = getAddrLocal(G, Ty, DAG);
      else
        Callee = getAddrGlobal(G, Ty, DAG, SDICII::MO_GOT_CALL, Chain,
                               FuncInfo->callPtrInfo(Val));
    } else
      Callee = DAG.getTargetGlobalAddress(G->getGlobal(), DL,
                                          getPointerTy(DAG.getDataLayout()), 0,
                                          SDICII::MO_NO_FLAG);
    GlobalOrExternal = true;
  }
  else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee)) {
    const char *Sym = S->getSymbol();

    if (!IsPIC) // static
      Callee = DAG.getTargetExternalSymbol(Sym,
                                           getPointerTy(DAG.getDataLayout()),
                                           SDICII::MO_NO_FLAG);
    else // PIC
      Callee = getAddrGlobal(S, Ty, DAG, SDICII::MO_GOT_CALL, Chain,
                             FuncInfo->callPtrInfo(Sym));

    GlobalOrExternal = true;
  }

  SmallVector<SDValue, 8> Ops(1, Chain);
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  getOpndList(Ops, RegsToPass, IsPICCall, GlobalOrExternal, InternalLinkage,
              CLI, Callee, Chain);

   //@TailCall 3 {
  if (IsTailCall)
    return DAG.getNode(SDICISD::TailCall, DL, MVT::Other, Ops);
  //@TailCall 3 }

  Chain = DAG.getNode(SDICISD::JmpLink, DL, NodeTys, Ops);
  SDValue InFlag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain, NextStackOffsetVal,
                             DAG.getIntPtrConstant(0, DL, true), InFlag, DL);
  InFlag = Chain.getValue(1);

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, IsVarArg,
                         Ins, DL, DAG, InVals, CLI.Callee.getNode(), CLI.RetTy);

  

  return CLI.Chain;
}
//@LowerCall }


/// LowerCallResult - Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
SDValue
SDICTargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag,
                                    CallingConv::ID CallConv, bool IsVarArg,
                                    const SmallVectorImpl<ISD::InputArg> &Ins,
                                    const SDLoc &DL, SelectionDAG &DAG,
                                    SmallVectorImpl<SDValue> &InVals,
                                    const SDNode *CallNode,
                                    const Type *RetTy) const {
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(),
		 RVLocs, *DAG.getContext());
		 
  SDICCC SDICCCInfo(CallConv, ABI.IsO32(), CCInfo);

  SDICCCInfo.analyzeCallResult(Ins, Subtarget.abiUsesSoftFloat(),
                               CallNode, RetTy);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    SDValue Val = DAG.getCopyFromReg(Chain, DL, RVLocs[i].getLocReg(),
                                     RVLocs[i].getLocVT(), InFlag);
    Chain = Val.getValue(1);
    InFlag = Val.getValue(2);

    if (RVLocs[i].getValVT() != RVLocs[i].getLocVT())
      Val = DAG.getNode(ISD::BITCAST, DL, RVLocs[i].getValVT(), Val);

    InVals.push_back(Val);
  }

  return Chain;
}



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

  SDICFI->setVarArgsFrameIndex(0);

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(),
                 ArgLocs, *DAG.getContext());
  SDICCC SDICCCInfo(CallConv, ABI.IsO32(), 
                    CCInfo);
  SDICFI->setFormalArgInfo(CCInfo.getNextStackOffset(),
                           SDICCCInfo.hasByValArg());

  Function::const_arg_iterator FuncArg =
    DAG.getMachineFunction().getFunction()->arg_begin();
  bool UseSoftFloat = Subtarget.abiUsesSoftFloat();

  SDICCCInfo.analyzeFormalArguments(Ins, UseSoftFloat, FuncArg);

  // Used with vargs to acumulate store chains.
  std::vector<SDValue> OutChains;

  unsigned CurArgIdx = 0;
  SDICCC::byval_iterator ByValArg = SDICCCInfo.byval_begin();

  //@2 {
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
  //@2 }
    CCValAssign &VA = ArgLocs[i];
    std::advance(FuncArg, Ins[i].OrigArgIndex - CurArgIdx);
    CurArgIdx = Ins[i].OrigArgIndex;
    EVT ValVT = VA.getValVT();
    ISD::ArgFlagsTy Flags = Ins[i].Flags;
    bool IsRegLoc = VA.isRegLoc();

    //@byval pass {
    if (Flags.isByVal()) {
      assert(Flags.getByValSize() &&
             "ByVal args of size 0 should have been ignored by front-end.");
      assert(ByValArg != SDICCCInfo.byval_end());
      copyByValRegs(Chain, DL, OutChains, DAG, Flags, InVals, &*FuncArg,
                    SDICCCInfo, *ByValArg);
      ++ByValArg;
      continue;
    }
    //@byval pass }
    // Arguments stored on registers
    if (ABI.IsO32() && IsRegLoc) {
      MVT RegVT = VA.getLocVT();
      unsigned ArgReg = VA.getLocReg();
      const TargetRegisterClass *RC = getRegClassFor(RegVT);

      // Transform the arguments stored on
      // physical registers into virtual ones
      unsigned Reg = addLiveIn(DAG.getMachineFunction(), ArgReg, RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegVT);

      // If this is an 8 or 16-bit value, it has been passed promoted
      // to 32 bits.  Insert an assert[sz]ext to capture this, then
      // truncate to the right size.
      if (VA.getLocInfo() != CCValAssign::Full) {
        unsigned Opcode = 0;
        if (VA.getLocInfo() == CCValAssign::SExt)
          Opcode = ISD::AssertSext;
        else if (VA.getLocInfo() == CCValAssign::ZExt)
          Opcode = ISD::AssertZext;
        if (Opcode)
          ArgValue = DAG.getNode(Opcode, DL, RegVT, ArgValue,
                                 DAG.getValueType(ValVT));
        ArgValue = DAG.getNode(ISD::TRUNCATE, DL, ValVT, ArgValue);
      }

      // Handle floating point arguments passed in integer registers.
      if ((RegVT == MVT::i32 && ValVT == MVT::f32) ||
          (RegVT == MVT::i64 && ValVT == MVT::f64))
        ArgValue = DAG.getNode(ISD::BITCAST, DL, ValVT, ArgValue);
      InVals.push_back(ArgValue);
    } else { // VA.isRegLoc()
      MVT LocVT = VA.getLocVT();

      // sanity check
      assert(VA.isMemLoc());

      // The stack pointer offset is relative to the caller stack frame.
      int FI = MFI->CreateFixedObject(ValVT.getSizeInBits()/8,
                                      VA.getLocMemOffset(), true);

      // Create load nodes to retrieve arguments from the stack
      SDValue FIN = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
      SDValue Load = DAG.getLoad(
          LocVT, DL, Chain, FIN,
          MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI));
      InVals.push_back(Load);
      OutChains.push_back(Load.getValue(1));
    }
  }

//@Ordinary struct type: 1 {
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    // The cpu0 ABIs for returning structs by value requires that we copy
    // the sret argument into $v0 for the return. Save the argument into
    // a virtual register so that we can access it from the return points.
    if (Ins[i].Flags.isSRet()) {
      unsigned Reg = SDICFI->getSRetReturnReg();
      if (!Reg) {
        Reg = MF.getRegInfo().createVirtualRegister(
            getRegClassFor(MVT::i32));
        SDICFI->setSRetReturnReg(Reg);
      }
      SDValue Copy = DAG.getCopyToReg(DAG.getEntryNode(), DL, Reg, InVals[i]);
      Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, Copy, Chain);
      break;
    }
  }
//@Ordinary struct type: 1 }

  // All stores are grouped in one node to allow the matching between
  // the size of Ins and InVals. This only happens when on varg functions
  if (!OutChains.empty()) {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, OutChains);
  }

  return Chain;
}
// @LowerFormalArguments }

//===----------------------------------------------------------------------===//
//@              Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

bool
SDICTargetLowering::CanLowerReturn(CallingConv::ID CallConv,
                                   MachineFunction &MF, bool IsVarArg,
                                   const SmallVectorImpl<ISD::OutputArg> &Outs,
                                   LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF,
                 RVLocs, Context);
  return CCInfo.CheckReturn(Outs, RetCC_SDIC);
}


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
   // SDValue Op0 = Chain.getOperand(0);
   

  //@Ordinary struct type: 2 }
 printf("\n'''''''''''''''''''''''''''''''''33333333333");
  RetOps[0] = Chain;
  // Update chain.
  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);
  // Return on SDIC is always a "ret $lr"
  return DAG.getNode(SDICISD::Ret, DL, MVT::Other,RetOps);
}

SDICTargetLowering::SDICCC::SpecialCallingConvType
  SDICTargetLowering::getSpecialCallingConv(SDValue Callee) const {
  SDICCC::SpecialCallingConvType SpecialCallingConv =
    SDICCC::NoSpecialCallingConv;
  return SpecialCallingConv;
}


SDICTargetLowering::SDICCC::SDICCC(
  CallingConv::ID CC, bool IsO32_, CCState &Info,
  SDICCC::SpecialCallingConvType SpecialCallingConv_)
  : CCInfo(Info), CallConv(CC), IsO32(IsO32_)
{
  // Pre-allocate reserved argument area.
  CCInfo.AllocateStack(reservedArgArea(), 1);
}


//@#if CH >= CH9_2 //6 {
void SDICTargetLowering::SDICCC::
analyzeCallOperands(const SmallVectorImpl<ISD::OutputArg> &Args,
                    bool IsVarArg, bool IsSoftFloat, const SDNode *CallNode,
                    std::vector<ArgListEntry> &FuncArgs) {
//@analyzeCallOperands body {
  assert((CallConv != CallingConv::Fast || !IsVarArg) &&
         "CallingConv::Fast shouldn't be used for vararg functions.");

  unsigned NumOpnds = Args.size();
  llvm::CCAssignFn *FixedFn = fixedArgFn();

  //@3 {
  for (unsigned I = 0; I != NumOpnds; ++I) {
  //@3 }
    MVT ArgVT = Args[I].VT;
    ISD::ArgFlagsTy ArgFlags = Args[I].Flags;
    bool R;

    if (ArgFlags.isByVal()) {
      handleByValArg(I, ArgVT, ArgVT, CCValAssign::Full, ArgFlags);
      continue;
    }

    {
      MVT RegVT = getRegVT(ArgVT, FuncArgs[Args[I].OrigArgIndex].Ty, CallNode,
                           IsSoftFloat);
      R = FixedFn(I, ArgVT, RegVT, CCValAssign::Full, ArgFlags, CCInfo);
    }

    if (R) {
#ifndef NDEBUG
      dbgs() << "Call operand #" << I << " has unhandled type "
             << EVT(ArgVT).getEVTString();
#endif
      llvm_unreachable(nullptr);
    }
  }
}
//@#if CH >= CH9_2 //6 }


//HYL for CALL
void SDICTargetLowering::SDICCC::
analyzeFormalArguments(const SmallVectorImpl<ISD::InputArg> &Args,
                       bool IsSoftFloat, Function::const_arg_iterator FuncArg) {
  unsigned NumArgs = Args.size();
  llvm::CCAssignFn *FixedFn = fixedArgFn();
  unsigned CurArgIdx = 0;

  for (unsigned I = 0; I != NumArgs; ++I) {
    MVT ArgVT = Args[I].VT;
    ISD::ArgFlagsTy ArgFlags = Args[I].Flags;
    std::advance(FuncArg, Args[I].OrigArgIndex - CurArgIdx);
    CurArgIdx = Args[I].OrigArgIndex;

    if (ArgFlags.isByVal()) {
      handleByValArg(I, ArgVT, ArgVT, CCValAssign::Full, ArgFlags);
      continue;
    }

    MVT RegVT = getRegVT(ArgVT, FuncArg->getType(), nullptr, IsSoftFloat);

    if (!FixedFn(I, ArgVT, RegVT, CCValAssign::Full, ArgFlags, CCInfo))
      continue;

#ifndef NDEBUG
    dbgs() << "Formal Arg #" << I << " has unhandled type "
           << EVT(ArgVT).getEVTString();
#endif
    llvm_unreachable(nullptr);
  }
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

//HYL for CALL
void SDICTargetLowering::SDICCC::handleByValArg(unsigned ValNo, MVT ValVT,
                                                MVT LocVT,
                                                CCValAssign::LocInfo LocInfo,
                                                ISD::ArgFlagsTy ArgFlags) {
  assert(ArgFlags.getByValSize() && "Byval argument's size shouldn't be 0.");

  struct ByValArgInfo ByVal;
  unsigned RegSize = regSize();
  unsigned ByValSize = alignTo(ArgFlags.getByValSize(), RegSize);
  unsigned Align = std::min(std::max(ArgFlags.getByValAlign(), RegSize),
                            RegSize * 2);

  if (useRegsForByval())
    allocateRegs(ByVal, ByValSize, Align);

  // Allocate space on caller's stack.
  ByVal.Address = CCInfo.AllocateStack(ByValSize - RegSize * ByVal.NumRegs,
                                       Align);
  CCInfo.addLoc(CCValAssign::getMem(ValNo, ValVT, ByVal.Address, LocVT,
                                    LocInfo));
  ByValArgs.push_back(ByVal);
}


//HYL for CALL
unsigned SDICTargetLowering::SDICCC::numIntArgRegs() const {
  return IsO32 ? array_lengthof(O32IntRegs) : 0;
}


unsigned SDICTargetLowering::SDICCC::reservedArgArea() const
{
  return (IsO32 && (CallConv != CallingConv::Fast)) ? 8 : 0;
}


const ArrayRef<MCPhysReg> SDICTargetLowering::SDICCC::intArgRegs() const {
  return makeArrayRef(O32IntRegs);
}

llvm::CCAssignFn *SDICTargetLowering::SDICCC::fixedArgFn() const {
  if (IsO32)
    return CC_SDICO32;
  else // IsS32
    return CC_SDICS32;
}

// HYL for CALL
void SDICTargetLowering::SDICCC::allocateRegs(ByValArgInfo &ByVal,
                                              unsigned ByValSize,
                                              unsigned Align) {
  unsigned RegSize = regSize(), NumIntArgRegs = numIntArgRegs();
  const ArrayRef<MCPhysReg> IntArgRegs = intArgRegs();
  assert(!(ByValSize % RegSize) && !(Align % RegSize) &&
         "Byval argument's size and alignment should be a multiple of"
         "RegSize.");

  ByVal.FirstIdx = CCInfo.getFirstUnallocated(IntArgRegs);

  // If Align > RegSize, the first arg register must be even.
  if ((Align > RegSize) && (ByVal.FirstIdx % 2)) {
    CCInfo.AllocateReg(IntArgRegs[ByVal.FirstIdx]);
    ++ByVal.FirstIdx;
  }

  // Mark the registers allocated.
  for (unsigned I = ByVal.FirstIdx; ByValSize && (I < NumIntArgRegs);
       ByValSize -= RegSize, ++I, ++ByVal.NumRegs)
    CCInfo.AllocateReg(IntArgRegs[I]);
}

MVT SDICTargetLowering::SDICCC::getRegVT(MVT VT, const Type *OrigTy,
					 const SDNode *CallNode,
					 bool IsSoftFloat) const
{
  if (IsSoftFloat || IsO32)
    return VT;
  return VT;
}
//HYL for CALL
void SDICTargetLowering::
copyByValRegs(SDValue Chain, const SDLoc &DL, std::vector<SDValue> &OutChains,
              SelectionDAG &DAG, const ISD::ArgFlagsTy &Flags,
              SmallVectorImpl<SDValue> &InVals, const Argument *FuncArg,
              const SDICCC &CC, const ByValArgInfo &ByVal) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  unsigned RegAreaSize = ByVal.NumRegs * CC.regSize();
  unsigned FrameObjSize = std::max(Flags.getByValSize(), RegAreaSize);
  int FrameObjOffset;

  const ArrayRef<MCPhysReg> ByValArgRegs = CC.intArgRegs();

  if (RegAreaSize)
    FrameObjOffset = (int)CC.reservedArgArea() -
      (int)((CC.numIntArgRegs() - ByVal.FirstIdx) * CC.regSize());
  else
    FrameObjOffset = ByVal.Address;

  // Create frame object.
  EVT PtrTy = getPointerTy(DAG.getDataLayout());
  int FI = MFI->CreateFixedObject(FrameObjSize, FrameObjOffset, true);
  SDValue FIN = DAG.getFrameIndex(FI, PtrTy);
  InVals.push_back(FIN);

  if (!ByVal.NumRegs)
    return;

  // Copy arg registers.
  MVT RegTy = MVT::getIntegerVT(CC.regSize() * 8);
  const TargetRegisterClass *RC = getRegClassFor(RegTy);

  for (unsigned I = 0; I < ByVal.NumRegs; ++I) {
    unsigned ArgReg = ByValArgRegs[ByVal.FirstIdx + I];
    unsigned VReg = addLiveIn(MF, ArgReg, RC);
    unsigned Offset = I * CC.regSize();
    SDValue StorePtr = DAG.getNode(ISD::ADD, DL, PtrTy, FIN,
                                   DAG.getConstant(Offset, DL, PtrTy)); 
    //SDValue Store = DAG.getStore(Chain, DL, DAG.getRegister(VReg, RegTy),
    //                           StorePtr, MachinePointerInfo(FuncArg, Offset));
    
    //HYL ADD
     SDValue OP1 = DAG.getNode(SDICISD::Movf, DL, MVT::Other, DAG.getRegister(VReg, RegTy));
    SDValue Store = DAG.getStore(Chain, DL, OP1,
                              StorePtr, MachinePointerInfo(FuncArg, Offset));
    
    OutChains.push_back(Store);
  }
}
