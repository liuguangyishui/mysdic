//===- SDICInstrInfo.td - Target Description for SDIC Target -*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the SDIC implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// SDIC profiles and nodes
//===----------------------------------------------------------------------===//

def SDT_SDICRet          : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
//ADD
def SDT_SDICAddwf        : SDTypeProfile<1, 2, [SDTCisInt<0>]>;
def SDT_SDICAddwftwo     : SDTypeProfile<1, 1, [SDTCisInt<0>]>;

//MOVLW K
def SDT_SDICMOVLW        : SDTypeProfile<1, 1, [SDTCisInt<0>]>;

def SDT_SDICPESUO        : SDTypeProfile<1, 1, [SDTCisInt<0>]>;
//CALL
def SDT_SDICJmpLink      : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;
def SDT_SDICCALL         : SDTypeProfile<1, 2, [SDTCisInt<0>]>;
def SDT_SDICPESUO_None   : SDTypeProfile<0, 1, [SDTCisInt<0>]>;

def SDT_SDICCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>]>;
def SDT_SDICCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;

// =====SDNode====================================================
// These are target-independent nodes, but have target-specific formats.
def callseq_start        : SDNode<"ISD::CALLSEQ_START", SDT_SDICCallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end          : SDNode<"ISD::CALLSEQ_END", SDT_SDICCallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

//Call
def SDICJmpLink          : SDNode<"SDICISD::JmpLink", SDT_SDICJmpLink,
                           [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

//Tail Call
def SDICTailCall         : SDNode<"SDICISD::TailCall", SDT_SDICJmpLink,
                                               [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDICMUL_CALL         : SDNode<"SDICISD::Mul_Call", SDT_SDICCALL, [SDNPOutGlue, SDNPInGlue]>;
def SDICDIV_CALL         : SDNode<"SDICISD::Div_Call", SDT_SDICCALL, [SDNPOutGlue, SDNPInGlue]>;
// Return
def SDICRet              : SDNode<"SDICISD::Ret", SDTNone, //SDT_SDICRet,//
                                              [ SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDICPesuo            : SDNode<"SDICISD::Pesuo", SDT_SDICPESUO, [SDNPOutGlue]>;
def SDICPesuo_NoRet      : SDNode<"SDICISD::Pesuo_NoRet", SDT_SDICRet, [SDNPOutGlue]>;
def SDICPesuo_None       : SDNode<"SDICISD::Pesuo_None", SDT_SDICPESUO_None,
                                                         [SDNPInGlue, SDNPOutGlue]>;
//MOVLW
def SDICMovf             : SDNode<"SDICISD::Movf", SDT_SDICMOVLW, [SDNPOutGlue]>;
def SDICMovlw            : SDNode<"SDICISD::Movlw", SDT_SDICMOVLW, [SDNPOutGlue]>;

//ADD
def SDICAddwf            : SDNode<"SDICISD::Addwf", SDT_SDICAddwf, [SDNPInGlue, SDNPOutGlue]>;
//R + R
def SDICAddwfrr          : SDNode<"SDICISD::Addwf", SDT_SDICAddwf , [SDNPInGlue, SDNPOutGlue]>;
//SUB
def SDICSubwf            : SDNode<"SDICISD::Subwf", SDT_SDICAddwf, [SDNPInGlue, SDNPOutGlue]>;

def SDICAddtest          : SDNode<"SDICISD::Addtest", SDTIntBinOp>;

// Hi and Lo nodes are used to handle global addresses. Used on
// SDICISelLowering to lower stuff like GlobalAddress, ExternalSymbol
// static model. (nothing to do with SDIC Registers Hi and Lo)
def SDICHi               : SDNode<"SDICISD::Hi", SDTIntUnaryOp>;
def SDICLo               : SDNode<"SDICISD::Lo", SDTIntUnaryOp>;
def SDICGPRel            : SDNode<"SDICISD::GPRel", SDTIntUnaryOp>;
def SDICWrapper          : SDNode<"SDICISD::Wrapper", SDTIntBinOp>;


//===----------------------------------------------------------------------===//
// SDIC Instruction Predicate Definitions.
//===----------------------------------------------------------------------===//

def Ch3_1       :     Predicate<"Subtarget->hasChapter3_1()">,
                      AssemblerPredicate<"FeatureChapter3_1">;
def Ch3_2       :     Predicate<"Subtarget->hasChapter3_2()">,
                      AssemblerPredicate<"FeatureChapter3_2">;
def Ch3_3       :     Predicate<"Subtarget->hasChapter3_3()">,
                      AssemblerPredicate<"FeatureChapter3_3">;
def Ch3_4       :     Predicate<"Subtarget->hasChapter3_4()">,
                      AssemblerPredicate<"FeatureChapter3_4">;
def Ch3_5       :     Predicate<"Subtarget->hasChapter3_5()">,
                      AssemblerPredicate<"FeatureChapter3_5">;
def Ch4_1       :     Predicate<"Subtarget->hasChapter4_1()">,
                      AssemblerPredicate<"FeatureChapter4_1">;
def Ch4_2       :     Predicate<"Subtarget->hasChapter4_2()">,
                      AssemblerPredicate<"FeatureChapter4_2">;
def Ch5_1       :     Predicate<"Subtarget->hasChapter5_1()">,
                      AssemblerPredicate<"FeatureChapter5_1">;
def Ch6_1       :     Predicate<"Subtarget->hasChapter6_1()">,
                      AssemblerPredicate<"FeatureChapter6_1">;
def Ch7_1       :     Predicate<"Subtarget->hasChapter7_1()">,
                      AssemblerPredicate<"FeatureChapter7_1">;
def Ch8_1       :     Predicate<"Subtarget->hasChapter8_1()">,
                      AssemblerPredicate<"FeatureChapter8_1">;
def Ch8_2       :     Predicate<"Subtarget->hasChapter8_2()">,
                      AssemblerPredicate<"FeatureChapter8_2">;
def Ch9_1       :     Predicate<"Subtarget->hasChapter9_1()">,
                      AssemblerPredicate<"FeatureChapter9_1">;
def Ch9_2       :     Predicate<"Subtarget->hasChapter9_2()">,
                      AssemblerPredicate<"FeatureChapter9_2">;
def Ch9_3       :     Predicate<"Subtarget->hasChapter9_3()">,
                      AssemblerPredicate<"FeatureChapter9_3">;
def Ch10_1      :     Predicate<"Subtarget->hasChapter10_1()">,
                      AssemblerPredicate<"FeatureChapter10_1">;
def Ch11_1      :     Predicate<"Subtarget->hasChapter11_1()">,
                      AssemblerPredicate<"FeatureChapter11_1">;
def Ch11_2      :     Predicate<"Subtarget->hasChapter11_2()">,
                      AssemblerPredicate<"FeatureChapter11_2">;
def Ch12_1      :     Predicate<"Subtarget->hasChapter12_1()">,
                      AssemblerPredicate<"FeatureChapter12_1">;
def Ch_all      :     Predicate<"Subtarget->hasChapterAll()">,
                      AssemblerPredicate<"FeatureChapterAll">;

def EnableOverflow  : Predicate<"Subtarget->enableOverflow()">;
def DisableOverflow : Predicate<"Subtarget->disableOverflow()">;

def HasCmp      :     Predicate<"Subtarget->hasCmp()">;
def HasSlt      :     Predicate<"Subtarget->hasSlt()">;

def RelocPIC    :     Predicate<"TM.getRelocationModel() == Reloc::PIC_">;

class SDICInstAlias<string Asm, dag Result, bit Emit = 0b1> :
      InstAlias<Asm, Result, Emit>;

class IsTailCall {
      bit isCall = 1;
      bit isTerminator = 1;
      bit isReturn = 1;
      bit isBarrier = 1;
      bit hasExtraSrcRegAllocReq = 1;
      bit isCodeGenOnly = 1;
      }

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//
include "SDICInstrFormats.td"
//===----------------------------------------------------------------------===//
// SDIC Operand, Complex Patterns and Transformations Definitions.
//===----------------------------------------------------------------------===//
// Instruction operand types

// Signed Operand
def simm8      : Operand<i32> {
    let DecoderMethod= "DecodeSimm8";
}

def simm16     : Operand<i32> {
    let DecoderMethod = "DecodeSimm16";
}

def shamt      :  Operand<i32>;

// Unsigned Operand
def uimm16 : Operand<i32> {
    let PrintMethod = "printUnsignedImm";
}

// Address operand
def mem : Operand<iPTR> {
    let PrintMethod = "printMemOperand";
    let MIOperandInfo = (ops SDICRegs, simm8);
    let EncoderMethod = "getMemEncoding";
}

def mem_ea : Operand<iPTR> {
    let PrintMethod = "printMemOperandEA";
    let MIOperandInfo = (ops SDICRegs, simm16);
    let EncoderMethod = "getMemEncoding";
}

// Transformation Function - get the lower 16 bits.
def LO16 : SDNodeXForm<imm, [{
    return getImm(N, N->getZExtValue() & 0xffff);
    }]>;
// Transformation Function - get the higher 16 bits.
def HI16 : SDNodeXForm<imm, [{
    return getImm(N, (N->getZExtValue() >> 16) & 0xffff);
    }]>;

// Node immediate fits as 16-bit sign extended on target immediate.
// e.g. addi, andi
def immSExt8  : PatLeaf<(imm), [{ return isInt<8>(N->getSExtValue()); }]>;

def immSExt16 : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;

def immSExt9  : PatLeaf<(imm), [{ return isInt<32>(N->getSExtValue()); }]>;
// Node immediate fits as 16-bit zero extended on target immediate.
// The LO16 param means that only the lower 16 bits of the node
// immediate are caught.
// e.g. addiu, sltiu
def immZExt16 : PatLeaf<(imm), [{
    if (N->getValueType(0) == MVT::i32)
    return (uint32_t)N->getZExtValue() == (unsigned short)N->getZExtValue();
    else
    return (uint64_t)N->getZExtValue() == (unsigned short)N->getZExtValue();
    }], LO16>;

// Immediate can be loaded with LUi (32-bit int with lower 16-bit cleared).
def immLow16Zero : PatLeaf<(imm), [{
    int64_t Val = N->getSExtValue();
    return isInt<32>(Val) && !(Val & 0xffff);
    }]>;

// shamt field must fit in 5 bits.
def immZExt5 : ImmLeaf<i32, [{return Imm == (Imm & 0x1f);}]>;

// SDIC Address Mode! SDNode frameindex could possibily be a match
// since load and store instructions from stack used it.
def addr : ComplexPattern<iPTR, 2, "SelectAddr", [frameindex], [SDNPWantParent]>;

def brtarget24     :  Operand<OtherVT> {
    let EncoderMethod = "getBranch24TargetOpValue";
    let OperandType = "OPERAND_PCREL";
    let DecoderMethod = "DecodeBranch24Target";
    }

//JMP
def jmptarget    : Operand<OtherVT>{
    let EncoderMethod = "getJumpTargetOpValue";
    let OperandType = "OPERAND_PCREL";
    }

def calltarget  : Operand<iPTR> {
    let  EncoderMethod = "getJumpTargetOpValue";
    }



//===----------------------------------------------------------------------===//
// Pattern fragment for load/store
//===----------------------------------------------------------------------===//

class AlignedLoad<PatFrag Node> :
  PatFrag<(ops node:$ptr), (Node node:$ptr), [{
  LoadSDNode *LD = cast<LoadSDNode>(N);
  return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlignment();
}]>;

class AlignedStore<PatFrag Node> :
  PatFrag<(ops node:$val, node:$ptr), (Node node:$val, node:$ptr), [{
  StoreSDNode *SD = cast<StoreSDNode>(N);
  return SD->getMemoryVT().getSizeInBits()/8 <= SD->getAlignment();
}]>;

// Load/Store PatFrags.
def load_a          : AlignedLoad<load>;
def store_a         : AlignedStore<store>;

//===----------------------------------------------------------------------===//
// Instructions specific format
//===----------------------------------------------------------------------===//
let Predicates = [Ch3_3] in {
// Arithmetic and logical instructions with 3 register operands.
class ArithLogicR<bits<6> op, string instr_asm, SDNode OpNode,
      InstrItinClass itin, RegisterClass RC, bit isComm = 0>:
   FF<op, (outs GPROut:$ra), (ins RC:$rb, RC:$rc),
          !strconcat(instr_asm, "\t$ra, $rb, $rc"), //留$rb
   [(set GPROut:$ra, (OpNode RC:$rb, RC:$rc))], itin> {
           let shamt = 0;
           let isCommutable = isComm;
           // e.g. add rb rc = add rc rb
           let isReMaterializable = 1;
     }
}

// Arithmetic and logical instructions with 2 register operands.
class ArithLogicI<bits<6> op, string instr_asm, SDNode OpNode,
                  Operand Od, PatLeaf imm_type, RegisterClass RC> :
  FA<op, (outs GPROut:$ra), (ins RC:$rb, Od:$rc),
     !strconcat(instr_asm, "\t$ra, $rb, $rc"), //留$rc
     [(set GPROut:$ra, (OpNode RC:$rb, imm_type:$rc))], IIAlu> {
  let isReMaterializable = 1;
}

//FDA_Link:three parameter and a node to link
class FDA_Link<SDNode OpNode, bits<6> op, string instr_asm,
                           RegisterClass RC>:
                      FA<op,(outs GPROut:$ra), (ins RC:$rb, RC:$rc),
                           !strconcat(instr_asm, "\t\t$rc"), //$ra $rb $rc  //\n\tmovwf \t$ra
                           [(set GPROut:$ra,(OpNode  RC:$rb, RC:$rc))], IIAddluwf> {
                          // let isReMaterializable = 1;     GPROut:$imma
                         // let imm8 = 0;
                          //let imma = 0;
                       }
//just three param FDA and no link node
class FDA_NoLink<SDNode OpNode, bits<6> op, string instr_asm,
                         RegisterClass RC> :
                      F_K<op, (outs GPROut:$shamt), (ins RC:$rk),
                              !strconcat(instr_asm, "\t\t$rk \t$shamt"), //留$rk
                             [(set GPROut:$shamt, (OpNode RC:$rk))], IIAlu> {

                           let isReMaterializable =1;
                       //    let shamt = 0;
}

//only one parameter K, but have aother node to link                    
class K_Link<SDNode OpNode, bits<6> op, string instr_asm,
                  Operand Od, PatLeaf imm_type, RegisterClass RC>:
                      FA<op,(outs GPROut:$ra), (ins  RC:$rb, Od:$rc),
                           !strconcat(instr_asm, "\t\t$rc \n\tmovwf\t\t$ra"),//\t$ra  $rb  $rc
               [(set GPROut:$ra,(OpNode  RC:$rb, imm_type:$rc))], IIAddluwf> {
               //GPROut:$imma
}
// only a immediate num K and a link for outs
class K_NoLink<SDNode OpNode, bits<6> op, string instr_asm,
                   Operand Od, PatLeaf imm_type>:
                           FA<op, (outs GPROut:$ra), (ins Od:$rb),
                                  !strconcat(instr_asm, "\t\t$rb \n\tmovwf \t\t$ra"),
                         [(set GPROut:$ra, (OpNode imm_type:$rb))], IIAlu>{
                               let rc = 0;
                         }

// for CALL调用子程序
class CALL_NoLink<SDNode OpNode, bits<6> op, string instr_asm,
                          RegisterClass RC>:
                     FI<op, (outs GPROut:$imma), (ins RC:$rn, RC:$link),
                            !strconcat(instr_asm, "\t"),
                      [(set GPROut:$imma, (OpNode RC:$rn, RC:$link))], IIAlu>{


                      }

//MY modify to movlw
class ArithLogicFK<SDNode OpNode, bits<6> op, string instr_asm,
                          Operand Od, PatLeaf imm_type> :
                      F_K<op, (outs GPROut:$shamt), (ins Od:$rk),
                              !strconcat(instr_asm, "\t$rk"),
                             [(set GPROut:$shamt, (OpNode imm_type:$rk))], IIAlu> {

                           let isReMaterializable =1;
                       //    let shamt = 0;
}


class ArithLogicFKR<SDNode OpNode, bits<6> op, string instr_asm,
                           RegisterClass RC>:
                        F_K<op, (outs GPROut:$shamt), (ins RC:$rk),
                           !strconcat(instr_asm, "\t$rk"),
                           [(set GPROut:$shamt, (OpNode RC:$rk))], IIAlu>{

                           
}

class ArithLogicPesuo<SDNode OpNode, string instr_asm, RegisterClass RC>:
                          SDICPseudo_I<(outs GPROut:$rb), (ins RC:$ra),//, RC:$rb),
                              !strconcat(instr_asm, "\t "),
                             [(set GPROut:$rb, (OpNode RC:$ra))]>
                             {
                                let isPseudo = 0;
                                   let isCodeGenOnly = 1;
                              }

class ArithLogicPesuo_NoRet<SDNode OpNode, string instr_asm, RegisterClass RC>:
                          SDICPseudo_I<(outs), (ins RC:$ra),//, RC:$rb),
                              !strconcat(instr_asm, "\t $ra"),
                             [(OpNode RC:$ra)]>
                             {

                              }




class SubSDICPseudo<SDNode OpNode, string instr_asm, RegisterClass RC>:
                           SDICPseudo<(outs), (ins RC:$ra),
                                    !strconcat(instr_asm, "\t$ra"),
                                    [(OpNode RC:$ra)]>
                                    {
                                   

}

class AriSDICPseudo_None<SDNode OpNode, string instr_asm, RegisterClass RC>:
                         SDICPseudo_None<(outs), (ins RC:$rb),
                             !strconcat(instr_asm, "\t"),
                             [ (OpNode RC:$rb)]>
                             {
                             let ra = 0;
                             //let rb = 0;
                            
                             }


//Move from Lo/Hi
class MoveFromLOHI<bits<6> op, string instr_asm, RegisterClass RC,
                           list<Register> UseRegs>:
                   FA<op, (outs RC:$ra),(ins),
                   !strconcat(instr_asm, "\t$ra"), [], IIHiLo> {
                   
                   let rb = 0;
                   let rc = 0;
                   let Uses = UseRegs;
                   let hasSideEffects = 0;

}

// Move to Lo/Hi
class MoveToLOHI<bits<6> op, string instr_asm, RegisterClass RC,
                 list<Register> DefRegs>:
                 FA<op, (outs), (ins RC:$ra),
                 !strconcat(instr_asm, "\t$ra"), [], IIHiLo> {
                   let rb = 0;
                   let rc = 0;
                   let Defs = DefRegs;
                   let hasSideEffects = 0;
}

class CmpInstr<bits<6> op, string instr_asm,
                       InstrItinClass itin, RegisterClass RC, RegisterClass RD,
                       bit isComm = 0>:
              FA<op, (outs RD:$ra), (ins RC:$rb, RC:$rc),
              !strconcat(instr_asm, "\t$rb, $rc"), [], itin> {//"\t$ra, $rb, $rc" 留的是$rb
              let isCommutable = isComm;
              let Predicates = [HasCmp];

              }
class CtrInstr2<bits<6> op, string instr_asm, InstrItinClass itin,
               RegisterClass RC, RegisterClass RD, Operand Od, PatLeaf imm_type>:
                 FF<op, (outs RD:$ra), (ins RC:$rb, RD:$rc, Od:$shamt),
                 !strconcat(instr_asm, "\t$rb, $shamt"), [], itin> {

               
               }

class CtrInstr<bits<6> op, string instr_asm, InstrItinClass itin,
              RegisterClass RC, RegisterClass RD, Operand Od, PatLeaf imm_type>:
                  FA<op, (outs RD:$ra), (ins RC:$rb, Od:$rc),
                  !strconcat(instr_asm, "\t$ra, $rc"), [], itin> {//\t$ra, $rb, $rc

                  }
class CtrInstr_BTFSC<bits<6> op, string instr_asm, InstrItinClass itin,
                     RegisterClass RC, Operand Od, PatLeaf imm_type>:
                        FA<op, (outs RC:$ra), (ins Od:$rc),
                        !strconcat(instr_asm, "\t$rb, $rc"), [], itin> {
                        let ra = 0;
                        }


//SetCC
class SetCC_R<bits<6> op, string instr_asm, PatFrag cond_op, RegisterClass RC>:
                      FA<op, (outs GPROut:$ra), (ins RC:$rb, RC:$rc),
                      !strconcat(instr_asm, "\t$ra, $rb, $rc"),
                      [(set GPROut:$ra, (cond_op RC:$rb, RC:$rc))],
                      IIAlu>, Requires<[HasSlt]> {

}

class SetCC_I<bits<6> op, string instr_asm, PatFrag cond_op, Operand Od,
                      PatLeaf imm_type, RegisterClass RC>:
                  FA<op, (GPROut:$ra), (ins RC:$rb, Od:$rc),
                  !strconcat(instr_asm, "\t$ra, $rb, $rc"),
                  [(set GPROut:$ra, (cond_op RC:$rb, imm_type:$rc))],
                  IIAlu>, Requires<[HasSlt]> {

}

//Unconditional branch , such as JMP
class UncondBranch<bits<6> op, string instr_asm>:
                           FJ<op, (outs), (ins jmptarget:$addr),
                                  !strconcat(instr_asm, "\t$addr"),
                                  [(br bb:$addr)], IIBranch> {
                           let isBranch = 1;
                           let isTerminator = 1;
                           let isBarrier = 1;
                           let hasDelaySlot = 0;
                              }

class CBranch24<bits<6> op, string instr_asm, RegisterClass RC,
                        list<Register> UseRegs>:
                    FJ<op, (outs), (ins RC:$ra, brtarget24:$addr),
                       !strconcat(instr_asm, "\t$addr"),//\t$ra, $addr
                        [], IIBranch>, Requires<[HasCmp]> {
                        let isBranch = 1;
                        let isTerminator = 1;
                        let hasDelaySlot = 1;
                        }

let Predicates = [Ch9_1] in {
// Jump and Link (Call)
let isCall = 1, hasDelaySlot = 1 in {
//@JumpLink {
class JumpLink<bits<6> op, string instr_asm>:
      FJ<op, (outs), (ins calltarget:$target, variable_ops),
      !strconcat(instr_asm, "\t$target"), [(SDICJmpLink imm:$target)],
      IIBranch> {
      // #if CH >= CH10_1 2
      // let DecoderMethod = "DecodeJumpTarget";
      // #endif
   }

//@JumpLink }

class JumpLinkReg<bits<6> op, string instr_asm,
                          RegisterClass RC>:
      FA<op, (outs), (ins RC:$rb, variable_ops),
      !strconcat(instr_asm, "\t$rb"), [(SDICJmpLink RC:$rb)], IIBranch> {
      let rc = 0;
      let ra = 14;
    //  let shamt = 0;
      }
}

//@class TailCall
let isCall = 1, isTerminator = 1, isReturn = 1, isBarrier = 1, hasDelaySlot = 1,
    hasExtraSrcRegAllocReq = 1, Defs = [R2] in {
  class TailCall<Instruction JumpInst> :
    PseudoSE<(outs), (ins calltarget:$target), [], IIBranch>,
    PseudoInstExpansion<(JumpInst jmptarget:$target)>;

  class TailCallReg<RegisterClass RO, Instruction JRInst,
                    RegisterClass ResRO = RO> :
    PseudoSE<(outs), (ins RO:$rs), [(SDICTailCall RO:$rs)], IIBranch>,
    PseudoInstExpansion<(JRInst ResRO:$rs)>;
}


}



// ===-----------------------------------------------------------------===//
// Pseudo instructions
// ===-----------------------------------------------------------------===//
let Predicates = [Ch9_2] in {
// As stack alignment is always done with addiu, we need a 16-bit immediate
let Defs = [R10], Uses = [R10] in {
def ADJCALLSTACKDOWN  : SDICPseudo<(outs), (ins uimm16:$amt),
                                   "!ADJCALLSTACKDOWN $amt",
                                   [(callseq_start timm:$amt)]>;
def ADJCALLSTACKUP    : SDICPseudo<(outs), (ins uimm16:$amt1, uimm16:$amt2),
                                    "!ADJCALLSTACKUP $amt1",
                                   [(callseq_end timm:$amt1, timm:$amt2)]>;
}

//@def CPRESTORE {
// When handling PIC code the assembler needs .cpload and .cprestore
// directives. If the real instructions corresponding these directives
// are used, we have the same behavior, but get also a bunch of warnings
// from the assembler.
let hasSideEffects = 0 in
def CPRESTORE : SDICPseudo<(outs), (ins i32imm:$loc, SDICRegs:$r9),
                                   ".cprestore\t$loc", []>;
} // let Predicates = [Ch9_2]


let Predicates = [Ch3_3] in {
// Shifts

class shift_rotate_imm<bits<6> op, bits<4> isRotate, string instr_asm,
                                SDNode OpNode, PatFrag PF, Operand ImmOpnd,
                                 RegisterClass RC>:
          FF<op, (outs GPROut:$ra), (ins RC:$rb, ImmOpnd:$shamt),
           !strconcat(instr_asm, "\t$ra, $rb, $shamt"),
           [(set GPROut:$ra, (OpNode RC:$rb, PF:$shamt))], IIAlu> {
           let rc = 0;
}

// 32-bit shift instructions.
class shift_rotate_imm32<bits<6> op, bits<4> isRotate, string instr_asm,
      SDNode OpNode>:
      shift_rotate_imm<op, isRotate, instr_asm, OpNode, immZExt5, shamt, SDICRegs>;
}


let Predicates = [Ch3_3] in {
// Load Upper Imediate
class LoadUpper<bits<6> op, string instr_asm, RegisterClass RC, Operand Imm>:
      FA<op, (outs RC:$ra), (ins Imm:$rb),
      !strconcat(instr_asm, "\t$ra, $rb"), [], IIAlu> {
      let rc = 0;
      let isReMaterializable = 1;
      }
}


class FMem<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern,
          InstrItinClass itin>: FE<op, outs, ins, asmstr, pattern, itin> {
  //bits<10> addr;
  bits<6> addr;
  let Inst{5-0} = addr{5-0};
  //let Inst{9-0}  = addr{9-0};
  let DecoderMethod = "DecodeMem";

}

// Memory Load/Store
let canFoldAsLoad = 1 in
class LoadM<bits<6> op, string instr_asm, PatFrag OpNode, RegisterClass RC,
            Operand MemOpnd, bit Pseudo>:
  FMem<op, (outs RC:$ra), (ins MemOpnd:$addr),
     !strconcat(instr_asm, "\t\t$addr \n\tmovwf \t\t$ra"), //(instr_asm, "\t$ra, $addr"),
     [(set RC:$ra, (OpNode addr:$addr))], IILoad> {
  let isPseudo = Pseudo;
}

class StoreM<bits<6> op, string instr_asm, PatFrag OpNode, RegisterClass RC,
             Operand MemOpnd, bit Pseudo>:
  FMem<op, (outs), (ins RC:$ra,MemOpnd:$addr),
     !strconcat(instr_asm, "\t\t$ra \n\tmovwf \t\t$addr"), //(instr_asm, "\t$ra, $addr"), 
     [(OpNode RC:$ra, addr:$addr)], IIStore> {   
  let isPseudo = Pseudo;
}

//@ 32-bit load.
multiclass LoadM32<bits<6> op, string instr_asm, PatFrag OpNode,
                   bit Pseudo = 0> {
  def #NAME# : LoadM<op, instr_asm, OpNode, SDICRegs, mem, Pseudo>; //GPROut,
}

// 32-bit store.
multiclass StoreM32<bits<6> op, string instr_asm, PatFrag OpNode,
                    bit Pseudo = 0> {
  def #NAME# : StoreM<op, instr_asm, OpNode, SDICRegs, mem, Pseudo>;
}

//@JumpFR {
let isBranch=1, isTerminator=1, isBarrier=1, rn=0, hasDelaySlot = 1,
    isIndirectBranch = 1 in
class JumpFR<bits<6> op, string instr_asm, RegisterClass RC>:
  FE<op, (outs), (ins RC:$rn),
     !strconcat(instr_asm, "\t$rn"), [(brind RC:$rn)], IIBranch> {
  let rn = 0;
 // let imm8 = 0;
}
//@JumpFR }


let Predicates = [Ch7_1] in {
//@EffectiveAddress
class EffectiveAddress<string instr_asm, RegisterClass RC, Operand Mem> :
      FMem<0x10, (outs RC:$rn), (ins Mem:$addr),
      instr_asm, [(set RC:$rn, addr:$addr)], IIAlu>;
      }


//hyl
let isBranch=1, isTerminator=1, isBarrier=1, ra=0, //rb=0, rc=0,
    hasDelaySlot=1,isIndirectBranch=1 in
class JumpFG<bits<6> op, string instr_asm, Operand Od, PatLeaf imm_type>:
      FG<op, (outs), (ins Od:$ra),//Od:$rb,Od:$rc),
      !strconcat(instr_asm,"\t$ra"),// rb, rc"),
      [(brind imm_type:$ra)], IIBranch>{

      
}
             
//hyl
class RetBaseFF<Operand Od>: JumpFG<0x3d, "ret",Od,immSExt8>{
  let isReturn = 1;
  let isCodeGenOnly = 1;
  let hasCtrlDep = 1;
  let hasExtraSrcRegAllocReq = 1;
  }
def RET       : RetBaseFF<simm8>;


// Return instruction
class RetBase<RegisterClass RC>: JumpFR<0x3c, "ret", RC> {
  let isReturn = 1;
  let isCodeGenOnly = 1;
  let hasCtrlDep = 1;
  let hasExtraSrcRegAllocReq = 1;
}

  
//===----------------------------------------------------------------------===//
// Instruction definition
//===----------------------------------------------------------------------===//
//===----------------------------------------------------------------------===//
// SDIC Instructions
//===----------------------------------------------------------------------===//

// ========== Load and Store Instructions and MOVF Instruction ==========
defm LD      : LoadM32<0x3d,  "movf(l)",  load_a>;  //ld
defm ST      : StoreM32<0x3e, "movf(s)",  store_a>;  //st
def Movlw    : K_NoLink<SDICMovlw, 0x2c, "movlw", simm8, immSExt8>;
def Movf     : FDA_NoLink<SDICMovf, 0x0d, "movf", SDICRegs>;
def Movwf    : FDA_NoLink<SDICMovf, 0x0e, "movwf", SDICRegs>;
//def Movlwk    : ArithLogicFK<SDICMovlw, 0x0d, "movlwk", simm8, immSExt8>;
def Movlwr   : ArithLogicFKR<SDICMovlw, 0x13, "movlwr", SDICRegs>;

// ========== Arithmetic Instructions  ==========
// add defined in include/llvm/Target/TargetSelectionDAG.td, line 315 (def add).
def ADDiu    : ArithLogicI<0x09, "addiu", SDICAddtest, simm8, immSExt8,SDICRegs>;
def ADDWF    : FDA_Link<SDICAddwf, 0x00, "addwf",  SDICRegs>;
def ADDLW    : K_Link<SDICAddwfrr, 0x28, "addlw", simm8, immSExt8, SDICRegs>;
//SUB
def SUBwf    : FDA_Link<SDICSubwf, 0x14, "subwf",  SDICRegs>;

//SUBWFB 带借位的减法
def SUBwfb   : FDA_Link<SDICSubwf, 0x15, "subwfb", SDICRegs>;
def SUBLW    : K_Link<SDICSubwf, 0x2e, "sublw", simm8, immSExt8, SDICRegs>;
// uncomplish
def MUL_CALL : CALL_NoLink<SDICMUL_CALL, 0x1d, "call\tMutiplication", SDICRegs>;
def DIV_CALL : CALL_NoLink<SDICDIV_CALL, 0x1c, "call\tDIV",  SDICRegs>;
def MUL      : ArithLogicI<0x17, "mul", mul, simm8, immSExt8, SDICRegs>;

// ========== Logic Instruction ==========
//AND
def ANDWF       : ArithLogicR<0x02, "andwf", and, IIAlu, SDICRegs, 1>;
def ANDLW       : ArithLogicI<0x29, "andlw", and, simm8, immSExt8, SDICRegs>;
//OR
def IORWF       : ArithLogicR<0x0c, "iorwf", or, IIAlu, SDICRegs, 1>;
def IORLW       : ArithLogicI<0x2a, "iorlw", or, simm8, immSExt8, SDICRegs>;
//XOR
def XORWF       :  ArithLogicR<0x17, "xorwf", xor, IIAlu, SDICRegs, 1>;
def XORLW       :  ArithLogicI<0x2f, "xorlw", xor, simm8, immSExt8, SDICRegs>;

//@def LEA_ADDiu {
// FrameIndexes are legalized when they are operands from load/store
// instructions. The same not happens for stack address copies, so an
// add op with mem ComplexPattern is used and the stack address copy
// can be matched. It's similar to Sparc LEA_ADDRi
def LEA_ADDiu : EffectiveAddress<"addiu\t$rn, $addr", SDICRegs, mem_ea> {
  let isCodeGenOnly = 1;
}
//@def LEA_ADDiu }



let Predicates = [HasCmp] in {
def CPFSEQ        : CmpInstr<0x0b, "cpfseq", IIAlu, SDICRegs, SDICRegs, 0>;
//def CPFSEQ2       : CmpInstr2<0x3b, "cpfseq", IIAlu, SDICRegs, SDICRegs, 0>;
}

// ========== JUMP Instruction ==========
//Unconditional branch   BRA
def BRA         :  UncondBranch<0x1c, "bra">;
let Predicates = [HasCmp] in {
def BRA2        :  CBranch24<0x1d, "bra", SDICRegs, [R9]>;
}

// ========== TEST Instruction ==========
// BSF 将寄存器中的某位置 1 
let Predicates = [HasCmp] in {
def BSF         :  CtrInstr<0x19, "bsf", IIAlu, SDICRegs, SDICRegs, simm8, immSExt8>;
def BSF2        :  CtrInstr2<0x39, "bsf", IIAlu, SR, SDICRegs, simm8, immSExt8>;
}
// BTFFSC 测试寄存器中的位，为 0 则跳过下条指令
def BTFSC       :  CtrInstr<0x1a, "btfsc", IIAlu, SDICRegs, SR, simm8, immSExt8>;
// BTFFSS 测试寄存器中的位，为 1 则跳过下条指令
def BTFSS       :  CtrInstr<0x1b, "btfss", IIAlu, SDICRegs, SR, simm8, immSExt8>;
/// Jump & link and Return Instructions
let Predicates = [Ch9_1] in {
//def JSUB        :  JumpLink<0x07, "jsub">;
def CALL        :  JumpLink<0x1d, "call">;
}
let Predicates = [Ch9_1] in {
def JALR        :  JumpLinkReg<0x08, "jalr", GPROut>;
}

//@ long branch support //1
let isBranch = 1, isTerminator = 1, isBarrier = 1,
    hasDelaySlot = 0, Defs = [R11] in
def BAL: FJ<0x3a, (outs), (ins jmptarget:$addr), "bal\t$addr", [], IIBranch>, 
             Requires<[HasSlt]>;

def JR      : JumpFR<0x3c, "jr", GPROut>;


def MFHI    : MoveFromLOHI<0x20, "mfhi", SDICRegs, [R5]>;
def MFLO    : MoveFromLOHI<0x21, "mflo", SDICRegs, [R6]>;
def MTHI    : MoveToLOHI<0x22, "mthi", SDICRegs, [R5]>;
def MTLO    : MoveToLOHI<0x23, "mtlo", SDICRegs, [R6]>;


let Predicates = [Ch9_1] in {
def TAILCALL : TailCall<BRA>;
def TAILCALL_R : TailCallReg<GPROut, JR>;
}



def Pesuo     : ArithLogicPesuo<SDICPesuo, "Pesuo", SDICRegs>;
def Pesuo_NoRet : ArithLogicPesuo_NoRet<SDICPesuo_NoRet, "Pesuo_NoRet", SDICRegs>;
def Pesuo_None     : AriSDICPseudo_None<SDICPesuo_None, "Pesuo_None", SDICRegs>;




let Predicates = [Ch3_3] in {
    def ORi     : ArithLogicI<0x1e, "ori", or, uimm16, immZExt16, SDICRegs>;
}

let Predicates = [Ch3_3] in {
    def LUi     : LoadUpper<0x0f, "lui", GPROut, uimm16>;
}
/*
/// Arithmetic Instructions (3-Operand, R-Type)
let Predicates = [Ch3_3] in {
let Predicates = [DisableOverflow] in {
def ADDu    : ArithLogicR<0x11, "addu", add, IIAlu, SDICRegs, 1>;
}
}
*/

/// Shift Instructions
let Predicates = [Ch3_3] in {
def SHL     : shift_rotate_imm32<0x1e, 0x00, "shl", shl>;
}

/// Arithmetic Instructions (3-Operand, R-Type)

/// Shift Instructions



let Predicates = [Ch3_3] in {
   let isReturn=1, isTerminator=1, hasDelaySlot=1, isBarrier=1, hasCtrlDep=1 in
      def RetLR : SDICPseudo<(outs),(ins ),"",[(SDICRet)]>;
     //  def RetLR : SubSDICPseudo<SDICRet, "re", SDICRegs>;
       }

//def RET     : RetBase<GPROut>;
//def RET       : RetBaseFF<simm8>;

/// No operation
let rn=0 in
  def NOP   : FE<0, (outs), (ins), "nop", [], IIAlu>;

/*
let Predicates = [Ch3_3] in {
//===----------------------------------------------------------------------===//
// Instruction aliases
//===----------------------------------------------------------------------===//

def : SDICInstAlias<"move $dst, $src",
                    (ADDu GPROut:$dst, GPROut:$src,R0), 1>;
}
*/

//===----------------------------------------------------------------------===//
//  Arbitrary patterns that map to one or more instructions
//===----------------------------------------------------------------------===//
multiclass addOperator<RegisterClass RC, PatLeaf Od> {

def : Pat<(add RC:$rl, RC:$rr),
               (ADDWF RC:$rl, RC:$rr)>;
def : Pat<(add RC:$re, Od:$in),
               (ADDLW RC:$re, Od:$in)>;

}

defm : addOperator<SDICRegs, immSExt16>;


// Small immediates
//def : Pat<(i32 immSExt8:$in),
 //        (Movlwk imm:$in)>;
//def : Pat<(ADDiu SDICRegs:$r9, (SDICMovlw imm:$in))>;

//def : Pat<(i32 immSExt8:$in),
  //            (Movlwk imm:$in)>;

def : Pat<(i32 immSExt16:$in),
         (Movlw imm:$in)>;

def : Pat<(i32 immSExt8:$in),
         //(ADDiu R0, imm:$in)>;
         (Movlw imm:$in)>;





let Predicates = [Ch3_3] in {
   def : Pat<(i32 immZExt16:$in),
             (ORi R0, imm:$in)>;
   def : Pat<(i32 immLow16Zero:$in),
             (LUi (HI16 imm:$in))>;

// Arbitrary immediates
def : Pat<(i32 imm:$imm),
                (ORi (LUi (HI16 imm:$imm)), (LO16 imm:$imm))>;
    } // let Predicates = [Ch3_4]


// hi/lo relocs
let Predicates = [Ch6_1] in {
def : Pat<(SDICHi tglobaladdr:$in), (LUi tglobaladdr:$in)>;
}

let Predicates = [Ch6_1] in {
def : Pat<(SDICLo tglobaladdr:$in), (ORi R2, tglobaladdr:$in)>;
}

let Predicates = [Ch6_1] in {
def : Pat<(add SDICRegs:$hi, (SDICLo tglobaladdr:$lo)),
          (ORi SDICRegs:$hi, tglobaladdr:$lo)>;
}
// gp_rel relocs
let Predicates = [Ch6_1] in {
def : Pat<(SDICAddwf SDICRegs:$r10, (SDICGPRel tglobaladdr:$in)),
          (ORi SDICRegs:$r10, tglobaladdr:$in)>;
}

//@ wrapper_pic
let Predicates = [Ch6_1] in {
class WrapperPat<SDNode node, Instruction ORiOp, RegisterClass RC>:
      Pat<(SDICWrapper RC:$r3, node:$in),
              (ORiOp RC:$r3, node:$in)>;

def : WrapperPat<tglobaladdr, ORi, GPROut>;
}
let Predicates = [Ch8_1] in {
def : WrapperPat<tjumptable, ORi, GPROut>;
}




multiclass SLOperator<RegisterClass RC>{

def : Pat<(load RC:$lhs),
                (CPFSEQ RC:$lhs, RC:$lhs)>;
}
defm : SLOperator<SDICRegs>;

// ========== COMPARE Instruction ==========
multiclass SeteqPatsCmp<RegisterClass RC>{
//A == B
    def :  Pat<(seteq RC:$lhs, RC:$rhs),
                  (BSF (CPFSEQ RC:$lhs, (Movf RC:$rhs)), 1)>;
    def :  Pat<(seteq RC:$lhs, immSExt8:$rhs),
                  (BSF (CPFSEQ  RC:$lhs, (Movlw immSExt8:$rhs)), 1)>;
//A != B
    def :  Pat<(setne RC:$lhs, RC:$rhs),
                  (BSF (CPFSEQ  RC:$lhs, RC:$rhs), 1)>;
}
//A < B
multiclass  SetltPatsCmp<RegisterClass RC> {
    def :  Pat<(setlt RC:$lhs, RC:$rhs),
                   (BTFSC (SUBwfb RC:$lhs, RC:$rhs), 0)>;
}
//A <= B
multiclass  SetlePatsCmp<RegisterClass RC> {
    def :  Pat<(setle RC:$lhs, RC:$rhs),
                  (BTFSC (BSF(CPFSEQ RC:$lhs, RC:$rhs), 1), 1)>;
}
// ========== Control Instruction ==========
multiclass BrcondPatsCmp<RegisterClass RC, Instruction CPFSEQOp,
                             Instruction BSFOp2, Instruction BSFOp,
                             Instruction BTFSSOp, Instruction BTFSCOp,
                             Instruction BRAOp2> {
//A == B
def : Pat<(brcond (i32 (setne RC:$lhs, RC:$rhs)), bb:$dst),
                  (BRAOp2 (CPFSEQOp RC:$lhs, (Movwf RC:$rhs)), bb:$dst)>;
//A != B
def : Pat<(brcond (i32 (seteq RC:$lhs, RC:$rhs)), bb:$dst),// bb:$dst),
    (BRAOp2 (BTFSSOp (CPFSEQOp (BSFOp2 RC:$lhs, SR, 0), (Movwf RC:$rhs)), 0), bb:$dst)>;
                  
//A < B 先借位减，C状态位为0，之后进行BTFSS测试，为1则跳过下条指令
def : Pat<(brcond (i32 (setlt RC:$lhs, RC:$rhs)), bb:$dst),
                  (BRAOp2 (BTFSSOp (SUBwfb RC:$lhs, (Movwf RC:$rhs)), 0), bb:$dst)>;
def : Pat<(brcond (i32 (setlt RC:$lhs, immSExt8:$rhs)), bb:$dst),
   (BRAOp2 (BTFSSOp (SUBwfb RC:$lhs, (Movwf (Movlw immSExt8:$rhs))), 0), bb:$dst)>;
//A > B
def : Pat<(brcond (i32 (setgt RC:$lhs, RC:$rhs)), bb:$dst),
                  (BRAOp2 (BTFSCOp (SUBwfb RC:$lhs, (Movwf RC:$rhs)), 0), bb:$dst)>;
def : Pat<(brcond (i32 (setgt RC:$lhs, immSExt8:$rhs)),bb:$dst),
   (BRAOp2 (BTFSSOp (SUBwfb RC:$lhs, (Movwf (Movlw immSExt8:$rhs))), 0), bb:$dst)>;
//A >= B
def : Pat<(brcond (i32 (setge RC:$lhs, RC:$rhs)), bb:$dst),
    (BRAOp2 (BTFSCOp (CPFSEQOp (SUBwfb RC:$lhs, RC:$rhs), RC:$rhs), 0), bb:$dst)>;
def : Pat<(brcond (i32 (setge RC:$lhs, immSExt8:$rhs)), bb:$dst),
    (BRAOp2 (BTFSCOp (CPFSEQOp (SUBwfb RC:$lhs, immSExt8:$rhs), immSExt8:$rhs), 0), bb:$dst)>;
//A <= B
def : Pat<(brcond (i32 (setle RC:$lhs, RC:$rhs)), bb:$dst),
    (BRAOp2 (BTFSCOp (CPFSEQOp (SUBwfb RC:$lhs, RC:$rhs), RC:$rhs), 0), bb:$dst)>;
def : Pat<(brcond (i32 (setle RC:$lhs, RC:$rhs)), bb:$dst),
    (BRAOp2 (BTFSCOp (CPFSEQOp (SUBwfb RC:$lhs, RC:$rhs), RC:$rhs), 0), bb:$dst)>;
    
//def : Pat<(brcond (i32 (set //  (BRAOp (BTFSCOp      ), bb:$dst)
def : Pat<(brcond RC:$cond, bb:$dst),
                  (BRAOp2 (CPFSEQOp RC:$cond, R6), bb:$dst)>;

}
let Predicates = [HasCmp] in {
defm : BrcondPatsCmp<SDICRegs, CPFSEQ, BSF2, BSF, BTFSS, BTFSC, BRA2>;
}

let Predicates = [HasCmp] in {
defm : SeteqPatsCmp<SDICRegs>;
defm : SetltPatsCmp<SDICRegs>;
}

let Predicates = [Ch9_1] in  {
def  : Pat<(SDICJmpLink (i32 tglobaladdr:$dst)),
       (CALL tglobaladdr:$dst)>;
def  : Pat<(SDICJmpLink (i32 texternalsym:$dst)),
       (CALL texternalsym:$dst)>;

//@Pat<SDICTailCall>
def  : Pat<(SDICTailCall (iPTR tglobaladdr:$dst)),
                         (TAILCALL tglobaladdr:$dst)>;

def  : Pat<(SDICTailCall (iPTR texternalsym:$dst)),
                         (TAILCALL texternalsym:$dst)>;
                         
}
