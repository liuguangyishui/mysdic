//===-- SDICISelLowering.h - SDIC DAG Lowering Interface --------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_SDIC_SDICISELLOWERING_H
#define LLVM_LIB_TARGET_SDIC_SDICISELLOWERING_H

#include "SDICConfig.h"

#include "MCTargetDesc/SDICBaseInfo.h"
#include "MCTargetDesc/SDICABIInfo.h"
#include "SDIC.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/IR/Function.h"
#include "llvm/Target/TargetLowering.h"
#include <deque>

namespace llvm {
  namespace SDICISD {
    enum NodeType {
      // Start the numbering from where ISD NodeType finishes.
      FIRST_NUMBER = ISD::BUILTIN_OP_END,

      // Jump and link (call)
      JmpLink,

      // Tail call
      TailCall,

      // Get the Higher 16 bits from a 32-bit immediate
      // No relation with SDIC Hi register
      Hi,
      // Get the Lower 16 bits from a 32-bit immediate
      // No relation with SDIC Lo register
      Lo,

      // Handle gp_rel (small data/bss sections) relocation.
      GPRel,

      // Thread Pointer
      ThreadPointer,

      // Return
      Ret,

     
      
      // Pesuo
      Pesuo,
      Pesuo_NoRet, //for fun that no ret value
      // Pesuo_None
      Pesuo_None,

      // Movlw
      Movlw,
      Movf,

      // Addwf
      Addwf,
      Addwfrr,
      // Subwf
      Subwf,
   

      // Addtest
      Addtest,
      //Call
      Mul_Call,
      Div_Call,
      // GPRel
      GPReltest,

      // Return
      Return,

      EH_RETURN,

      // DivRem(u)
      DivRem,
      DivRemU,

      Wrapper,
      DynAlloc,

      Sync
    };
  }

  //===--------------------------------------------------------------------===//
  // TargetLowering Implementation
  //===--------------------------------------------------------------------===//
  class SDICFunctionInfo;
  class SDICSubtarget;

  //@class SDICTargetLowering
  class SDICTargetLowering : public TargetLowering  {
  public:
    explicit SDICTargetLowering(const SDICTargetMachine &TM,
                                const SDICSubtarget &STI);

    static const SDICTargetLowering *create(const SDICTargetMachine &TM,
                                            const SDICSubtarget &STI);

    /// getTargetNodeName - This method returns the name of a target specific
    //  DAG node.
    const char *getTargetNodeName(unsigned Opcode) const override;

     /// getSetCCResultType - get the ISD::SETCC result ValueType
    EVT getSetCCResultType(const DataLayout &DL, LLVMContext &Context,
                           EVT VT) const override;

    //LowerOperation -Provide custom lowering hooks for some operations.
    virtual SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const;

    
    SDValue LowerADD(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerMUL(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerDIV(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerSUB(SDValue Op, SelectionDAG &DAG) const;

    SDValue LowerSTORE(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerLOAD(SDValue Op, SelectionDAG &DAG) const;
    
    SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const override;
    
  protected:
     SDValue getGlobalReg(SelectionDAG &DAG, EVT Ty) const;
    // This method creates the following nodes, which are necessary for
    // computing a local symbol's address:
    //
    // (add (load (wrapper $gp, %got(sym)), %lo(sym))
    template<class NodeTy>
    SDValue getAddrLocal(NodeTy *N, EVT Ty, SelectionDAG &DAG) const {
      SDLoc DL(N);
      unsigned GOTFlag = SDICII::MO_GOT;
      SDValue GOT = DAG.getNode(SDICISD::Wrapper, DL, Ty, getGlobalReg(DAG, Ty),
                                getTargetNode(N, Ty, DAG, GOTFlag));
      SDValue Load =
          DAG.getLoad(Ty, DL, DAG.getEntryNode(), GOT,
                      MachinePointerInfo::getGOT(DAG.getMachineFunction()));
      unsigned LoFlag = SDICII::MO_ABS_LO;
      SDValue Lo = DAG.getNode(SDICISD::Lo, DL, Ty,
                               getTargetNode(N, Ty, DAG, LoFlag));
      return DAG.getNode(ISD::ADD, DL, Ty, Load, Lo);
    }

    //@getAddrGlobal {
    // This method creates the following nodes, which are necessary for
    // computing a global symbol's address:
    //
    // (load (wrapper $gp, %got(sym)))
    template<class NodeTy>
    SDValue getAddrGlobal(NodeTy *N, EVT Ty, SelectionDAG &DAG,
                          unsigned Flag, SDValue Chain,
                          const MachinePointerInfo &PtrInfo) const {
      SDLoc DL(N);
      SDValue Tgt = DAG.getNode(SDICISD::Wrapper, DL, Ty, getGlobalReg(DAG, Ty),
                                getTargetNode(N, Ty, DAG, Flag));
      return DAG.getLoad(Ty, DL, Chain, Tgt, PtrInfo);
    }
    //@getAddrGlobal }


    //@getAddrGlobalLargeGOT {
    // This method creates the following nodes, which are necessary for
    // computing a global symbol's address in large-GOT mode:
    //
    // (load (wrapper (add %hi(sym), $gp), %lo(sym)))
    template<class NodeTy>
    SDValue getAddrGlobalLargeGOT(NodeTy *N, EVT Ty, SelectionDAG &DAG,
                                  unsigned HiFlag, unsigned LoFlag,
                                  SDValue Chain,
                                  const MachinePointerInfo &PtrInfo) const {
      SDLoc DL(N);
      SDValue Hi = DAG.getNode(SDICISD::Hi, DL, Ty,
                               getTargetNode(N, Ty, DAG, HiFlag));
      Hi = DAG.getNode(ISD::ADD, DL, Ty, Hi, getGlobalReg(DAG, Ty));
      SDValue Wrapper = DAG.getNode(SDICISD::Wrapper, DL, Ty, Hi,
                                    getTargetNode(N, Ty, DAG, LoFlag));
      return DAG.getLoad(Ty, DL, Chain, Wrapper, PtrInfo);
    }
    //@getAddrGlobalLargeGOT }


     //@getAddrNonPIC
    // This method creates the following nodes, which are necessary for
    // computing a symbol's address in non-PIC mode:
    //
    // (add %hi(sym), %lo(sym))
    template<class NodeTy>
    SDValue getAddrNonPIC(NodeTy *N, EVT Ty, SelectionDAG &DAG) const {
      SDLoc DL(N);
      SDValue Hi = getTargetNode(N, Ty, DAG, SDICII::MO_ABS_HI);
      SDValue Lo = getTargetNode(N, Ty, DAG, SDICII::MO_ABS_LO);
      return DAG.getNode(ISD::ADD, DL, Ty,
                         DAG.getNode(SDICISD::Hi, DL, Ty, Hi),
                         DAG.getNode(SDICISD::Lo, DL, Ty, Lo));
    }
    

    /// This function fills Ops, which is the list of operands that will later
    /// be used when a function call node is created. It also generates
    /// copyToReg nodes to set up argument registers.
    virtual void
    getOpndList(SmallVectorImpl<SDValue> &Ops,
                std::deque< std::pair<unsigned, SDValue> > &RegsToPass,
                bool IsPICCall, bool GlobalOrExternal, bool InternalLinkage,
                CallLoweringInfo &CLI, SDValue Callee, SDValue Chain) const;




    
    /// ByValArgInfo - Byval argument information.
    struct ByValArgInfo {
      unsigned FirstIdx; // Index of the first register used.
      unsigned NumRegs;  // Number of registers used for this argument.
      unsigned Address;  // Offset of the stack area used to pass this argument.

      ByValArgInfo() : FirstIdx(0), NumRegs(0), Address(0) {}
    };


/// SDICCC - This class provides methods used to analyze formal and call
/// arguments and inquire about calling convention information.
    class SDICCC {
public:
      enum SpecialCallingConvType {
	NoSpecialCallingConv
      };

      SDICCC(CallingConv::ID CallConv, bool IsO32, CCState &Info,
	     SpecialCallingConvType SpecialCallingConv = NoSpecialCallingConv);

      void analyzeCallOperands(const SmallVectorImpl<ISD::OutputArg> &Outs,
                               bool IsVarArg, bool IsSoftFloat,
                               const SDNode *CallNode,
                               std::vector<ArgListEntry> &FuncArgs);
      
      void analyzeFormalArguments(const SmallVectorImpl<ISD::InputArg> &Ins,
				  bool IsSoftFloat,
				  Function::const_arg_iterator FuncArg);
      
      void analyzeCallResult(const SmallVectorImpl<ISD::InputArg> &Ins,
			     bool IsSoftFloat, const SDNode *CallNode,
			     const Type *RetTy) const;

      void analyzeReturn(const SmallVectorImpl<ISD::OutputArg> &Outs,
			 bool IsSoftFloat, const Type *RetTy) const;

      const CCState &getCCInfo() const { return CCInfo; }

      // hasByValArg - Returns true if function has byval arguments.
      bool hasByValArg() const { return !ByValArgs.empty(); }

      ///regSize - Size (in number of bits) of integet registers.
      unsigned regSize() const { return IsO32 ? 4 : 4; }
      /// numIntArgRegs - Number of integer registers available for calls.
      unsigned numIntArgRegs() const;
      
      /// reservedArgArea - The size of the area the caller reserves for
      /// register arguments. This is 16-byte if ABI is O32.
      unsigned reservedArgArea() const;

      /// Return pointer to array of integer argument registers.
      const ArrayRef<MCPhysReg> intArgRegs() const;
      
      typedef SmallVectorImpl<ByValArgInfo>::const_iterator byval_iterator;
      byval_iterator byval_begin() const { return ByValArgs.begin(); }

      byval_iterator byval_end() const { return ByValArgs.end(); }

    private:
      
      void handleByValArg(unsigned ValNo, MVT ValVT, MVT LocVT,
                          CCValAssign::LocInfo LocInfo,
                          ISD::ArgFlagsTy ArgFlags);
       /// useRegsForByval - Returns true if the calling convention allows the
      /// use of registers to pass byval arguments.
      bool useRegsForByval() const { return CallConv != CallingConv::Fast; }

      /// Return the function that analyzes fixed argument list functions.
      llvm::CCAssignFn *fixedArgFn() const;

      void allocateRegs(ByValArgInfo &ByVal, unsigned ByValSize,
                        unsigned Align);
      
      ///Return the type of the register which is used to pass an argument or
      ///return a value. This function returns f64 if the argument is an i64
      ///value which has been generated as a result of softening an f128 value.
      ///Otherwise, it just returns VT.
      MVT getRegVT(MVT VT, const Type *OrigTy, const SDNode *CallNode,
		   bool IsSoftFloat) const;

      template<typename Ty>
      void analyzeReturn(const SmallVectorImpl<Ty> &RetVals, bool IsSoftFloat,
			 const SDNode *CallNode, const Type *RetTy) const;
      CCState &CCInfo;
      CallingConv::ID CallConv;
      bool IsO32;
      SmallVector<ByValArgInfo, 2> ByValArgs;
    };



    
  protected:
    // Subtarget Info
    const SDICSubtarget &Subtarget;
    // Cache the ABI from the TargetMachine, we use it everywhere.
    const SDICABIInfo &ABI;

  private:

     // Create a TargetGlobalAddress node.
    SDValue getTargetNode(GlobalAddressSDNode *N, EVT Ty, SelectionDAG &DAG,
                          unsigned Flag) const;

    // Create a TargetExternalSymbol node.
    SDValue getTargetNode(ExternalSymbolSDNode *N, EVT Ty, SelectionDAG &DAG,
                          unsigned Flag) const;

    // Create a TargetBlockAddress node.
    SDValue getTargetNode(BlockAddressSDNode *N, EVT Ty, SelectionDAG &DAG,
                          unsigned Flag) const;

    // Create a TargetJumpTable node.
    SDValue getTargetNode(JumpTableSDNode *N, EVT Ty, SelectionDAG &DAG,
                          unsigned Flag) const;
    
    SDICCC::SpecialCallingConvType getSpecialCallingConv(SDValue Callee) const;

       // Lower Operand helpers
    SDValue LowerCallResult(SDValue Chain, SDValue InFlag,
                            CallingConv::ID CallConv, bool isVarArg,
                            const SmallVectorImpl<ISD::InputArg> &Ins,
                            const SDLoc &dl, SelectionDAG &DAG,
                            SmallVectorImpl<SDValue> &InVals,
                            const SDNode *CallNode, const Type *RetTy) const;


    // Lower Operand specifics
    SDValue lowerBR_JT(SDValue Op, SelectionDAG &DAG) const;
    SDValue lowerBRCOND(SDValue Op, SelectionDAG &DAG) const;
    SDValue lowerBlockAddress(SDValue Op, SelectionDAG &DAG) const;
    SDValue lowerJumpTable(SDValue Op, SelectionDAG &DAG) const;
    SDValue lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
    SDValue lowerSELECT(SDValue Op, SelectionDAG &DAG) const;
    
    /// isEligibleForTailCallOptimization - Check whether the call is eligible
    /// for tail call optimization.
    virtual bool
    isEligibleForTailCallOptimization(const SDICCC &SDICCCInfo,
                                      unsigned NextStackOffset,
                                      const SDICFunctionInfo& FI) const = 0;

    /// copyByValArg - Copy argument registers which were used to pass a byval
    /// argument to the stack. Create a stack frame object for the byval
    /// argument.
    void copyByValRegs(SDValue Chain, const SDLoc &DL,
                       std::vector<SDValue> &OutChains, SelectionDAG &DAG,
                       const ISD::ArgFlagsTy &Flags,
                       SmallVectorImpl<SDValue> &InVals,
                       const Argument *FuncArg,
                       const SDICCC &CC, const ByValArgInfo &ByVal) const;

    
    /// passByValArg - Pass a byval argument in registers or on stack.
    void passByValArg(SDValue Chain, const SDLoc &DL,
                      std::deque< std::pair<unsigned, SDValue> > &RegsToPass,
                      SmallVectorImpl<SDValue> &MemOpChains, SDValue StackPtr,
                      MachineFrameInfo *MFI, SelectionDAG &DAG, SDValue Arg,
                      const SDICCC &CC, const ByValArgInfo &ByVal,
                      const ISD::ArgFlagsTy &Flags, bool isLittle) const;

   
  
    
    

	//- must be exist even without function all
    SDValue
      LowerFormalArguments(SDValue Chain,
                           CallingConv::ID CallConv, bool IsVarArg,
                           const SmallVectorImpl<ISD::InputArg> &Ins,
                           const SDLoc &dl, SelectionDAG &DAG,
                           SmallVectorImpl<SDValue> &InVals) const override;

    SDValue passArgOnStack(SDValue StackPtr, unsigned Offset, SDValue Chain,
                           SDValue Arg, const SDLoc &DL, bool IsTailCall,
                           SelectionDAG &DAG) const;

    SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                      SmallVectorImpl<SDValue> &InVals) const override;

    bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                        bool isVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
			  LLVMContext &Context) const override;
    
    SDValue LowerReturn(SDValue Chain,
                        CallingConv::ID CallConv, bool IsVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        const SmallVectorImpl<SDValue> &OutVals,
                        const SDLoc &dl, SelectionDAG &DAG) const override;

    bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const override;
  };
  const SDICTargetLowering *
  createSDICSETargetLowering(const SDICTargetMachine &TM, const SDICSubtarget &STI);
}

#endif // SDICISELLOWERING_H

