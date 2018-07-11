//===-- SDICCallingConv.td - Calling Conventions for SDIC --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for SDIC architecture.
//===----------------------------------------------------------------------===//

/// CCIfSubtarget - Match if the current subtarget has a feature F.
class CCIfSubtarget<string F, CCAction A>:
  CCIf<!strconcat("State.getTarget().getSubtarget<SDICSubtarget>().", F), A>;

def RetCC_SDICEABI : CallingConv<[
    // i32 are returned in registers R8
    //CCIfType<[i32], CCAssignToReg<[R8,R11,R12]>>]>;
    CCIfType<[i32], CCAssignToReg<[R0, R1]>>]>;

def RetCC_SDIC      :   CallingConv<[
    CCDelegateTo<RetCC_SDICEABI>]>;



def CSR_O32 : CalleeSavedRegs<(add R9, TOSL, TOSH)>;
                                  // (sequence "S%u", 1, 0))>;


