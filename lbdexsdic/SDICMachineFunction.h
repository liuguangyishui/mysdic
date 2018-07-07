//===-- SDICMachineFunctionInfo.h - Private data used for SDIC ----*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the SDIC specific subclass of MachineFunctionInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDICMACHINEFUNCTION_H
#define LLVM_LIB_TARGET_SDIC_SDICMACHINEFUNCTION_H

#include "SDICConfig.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include <map>

namespace llvm {

//@1 {
/// SDICFunctionInfo - This class is derived from MachineFunction private
/// SDIC target-specific information for each MachineFunction.
class SDICFunctionInfo : public MachineFunctionInfo {
public:
  SDICFunctionInfo(MachineFunction& MF)
  : MF(MF),
    SRetReturnReg(0),CallsEhReturn(false),CallsEhDwarf(false),
    VarArgsFrameIndex(0),
    InArgFIRange(std::make_pair(-1, 0)),
    OutArgFIRange(std::make_pair(-1, 0)), GPFI(0), DynAllocFI(0),
    EmitNOAT(false),
    MaxCallFrameSize(0)
    {}

  ~SDICFunctionInfo();

   bool isInArgFI(int FI) const {
    return FI <= InArgFIRange.first && FI >= InArgFIRange.second;
  }
   void setLastInArgFI(int FI) { InArgFIRange.second = FI; }
  bool isOutArgFI(int FI) const {
    return FI <= OutArgFIRange.first && FI >= OutArgFIRange.second;
  }

  int getGPFI() const { return GPFI; }
  void setGPFI(int FI) { GPFI = FI; }
  bool isGPFI(int FI) const { return GPFI && GPFI == FI; }

  bool isDynAllocFI(int FI) const { return DynAllocFI && DynAllocFI == FI; }
  
  
  unsigned getSRetReturnReg() const { return SRetReturnReg;}
  void setSRetReturnReg(unsigned Reg){ SRetReturnReg = Reg; }

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

  bool hasByvalArg() const { return HasByvalArg;}
  void setFormalArgInfo(unsigned Size, bool HasByval)
  {
    IncomingArgSize = Size;
    HasByvalArg = HasByval;
  }

  unsigned getIncomingArgSize() const { return IncomingArgSize; }

  bool callsEhReturn() const { return CallsEhReturn; }
  void setCallsEhReturn() { CallsEhReturn = true; }

  bool callsEhDwarf() const { return CallsEhDwarf; }
  void setCallsEhDwarf() { CallsEhDwarf = true; }

  void createEhDataRegsFI();
  int getEhDataRegFI(unsigned Reg) const { return EhDataRegFI[Reg]; }

  unsigned getMaxCallFrameSize() const { return MaxCallFrameSize; }
  void setMaxCallFrameSize(unsigned S) { MaxCallFrameSize = S; }
  
  bool getEmitNOAT() const { return EmitNOAT; }
  void setEmitNOAT() { EmitNOAT = true; }

private:
  virtual void anchor();

  MachineFunction& MF;

  /// SRetReturnReg - Some subtargets require that sret lowering includes
  /// returning the value of the returned struct in a register. This field
  /// holds the virtual register into which the sret argument is passed.
  unsigned SRetReturnReg;
  
  // VarArgsFrameIndex - FrameIndex for start of varargs area.
  int VarArgsFrameIndex;

  /// True if function has a byval argument.
  bool HasByvalArg;

  /// Size of incoming argument area.
  unsigned IncomingArgSize;

  /// CallsEhReturn - Whether the function calls llvm.eh.return.
  bool CallsEhReturn;
  
  /// CallsEhDwarf - Whether the function calls llvm.eh.dwarf.
  bool CallsEhDwarf;
  
  /// Frame objects for spilling eh data registers.
  int EhDataRegFI[2];

  // Range of frame object indices.
  // InArgFIRange: Range of indices of all frame objects created during call to
  //               LowerFormalArguments.
  // OutArgFIRange: Range of indices of all frame objects created during call to
  //                LowerCall except for the frame object for restoring $gp.
  std::pair<int, int> InArgFIRange, OutArgFIRange;

  int GPFI; // Index of the frame object for restoring $gp
  mutable int DynAllocFI; // Frame index of dynamically allocated stack area.
  bool EmitNOAT;
  unsigned MaxCallFrameSize;
};
//@1 }

} // end of namespace llvm

#endif // SDIC_MACHINE_FUNCTION_INFO_H

