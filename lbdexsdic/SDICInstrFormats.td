//===-- SDICInstrFormats.td - SDIC Instruction Formats -----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe CPU0 instructions format
//
//  CPU INSTRUCTION FORMATS
//
//  opcode  - operation code.
//  ra      - dst reg, only used on 3 regs instr.
//  rb      - src reg.
//  rc      - src reg (on a 3 reg instr).
//  cx      - immediate
//
//===----------------------------------------------------------------------===//


// Format specifies the encoding used by the instruction. This is part of
// the ad-hoc solution used to emit machine instruction encodings by our
// machine code emitter.
class Format<bits<4> val> {
      bits<4> Value = val;
      }

def Pseudo      : Format<0>;
def FrmA        : Format<1>;
def FrmA_a      : Format<9>;
def FrmB        : Format<2>;
def FrmC        : Format<3>;
def FrmD        : Format<4>;
def FrmE        : Format<5>;
def FrmF        : Format<6>;
def FrmG        : Format<7>;//only one operand ie: MOVLW K
def FrmH        : Format<8>;//


//Generic SDIC Format
class SDICInst<dag outs, dag ins, string asmstr, list<dag> pattern,
                   InstrItinClass itin, Format f> : Instruction
                   {
                   field bits<16> Inst; //说明指令有16位
                   Format Form = f; //代表的是哪一类指令

                   let Namespace = "SDIC";
                   let Size = 4;

                   bits<6> Opcode = 0;
                    
                    //Top 8 bits are the 'opcode' field
                    let Inst{15-10} = Opcode;

                    let OutOperandList = outs;
                    let InOperandList = ins;

                    let AsmString = asmstr;
                    let Pattern = pattern;
                    let Itinerary = itin;
                    //
                    //Attributes specific to SDIC instructions..
                    bits<4> FormBits = Form.Value;

                    //TSFlags layout should be kept in sync with SDICInstrInfo.h
                    let TSFlags{3-0} = FormBits;
                    let DecoderNamespace = "SDIC";
                    field bits<16> SoftFail = 0;
                    }
                    

// SDIC Pseudo Instructions Format
class SDICPseudo<dag outs, dag ins, string asmstr, list<dag> pattern>:
      SDICInst<outs, ins, asmstr, pattern, IIPseudo, Pseudo> {
                  //let isCodeGenOnly=1;
                  //let isPseudo=1;
                  bits<1> ra;
                  let Inst{9-9} = ra;
                  }


class SDICPseudo_I<dag outs, dag ins, string asmstr, list<dag> pattern>:
      SDICInst<outs, ins, asmstr, pattern, IIPseudo, Pseudo>
      {
        bits<6> ra;
     //   bits<1> rb;
        //bits<1> rc;
      //  bits<2> rd;

        let Inst{9-4} = ra;
       // let Inst{3-3} = rb;
       // let Inst{2-2} = rc;
       // let Inst{1-0} = rd;
              //   let isPseudo = 1;
      }

class SDICPseudo_None<dag outs, dag ins, string asmstr, list<dag> pattern>:
      SDICInst<outs, ins, asmstr, pattern, IIPseudo, Pseudo>
      {
       

      }
//===---------------------------------------------------------------===//
// Format A instruction class in SDIC : <|opcode|F|D|A|>
//===---------------------------------------------------------------===//
 class FA<bits<6> op, dag outs, dag ins, string asmstr,
                    list<dag> pattern, InstrItinClass itin>:
                    SDICInst<outs, ins, asmstr, pattern, itin, FrmA>
        {
         bits<8> rf;      // 对应F，F是8位寄存器地址
         bits<1> rd;      // 对应D, D是目标寄存器选择位 1位
         bits<1> imm8;      // 对应A, A是快速操作RAM位 1位

         let Opcode = op;
         let Inst{9-2} = rf;
         let Inst{1-1} = rd;
         let Inst{0-0} = imm8;
         }

//===--------------------------------------------------------------===//
// Format A_a instruction class will be the formal instruction of A
//===--------------------------------------------------------------===//
 class FA_a<bits<6> op, dag outs, dag ins, string asmstr,
                    list<dag> pattern, InstrItinClass itin>:
                    SDICInst<outs, ins, asmstr, pattern, itin, FrmA_a>
                    {
                        bits<8> rf;
                        bits<1> imm8;
                        bits<1> imma;

                        let Opcode = op;
                        let Inst{9-2} = rf;
                        let Inst{1-1} = imm8;
                        let Inst{0-0} = imma;
                    }


// Format B instruction class in SDIC : <|opcode|F|A|>
// 这是F A类型的指令
 class FB<bits<6> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern, InstrItinClass itin>:
                  SDICInst<outs, ins, asmstr, pattern, itin, FrmB>
         {
          bits<8> rf;   //对应F, F是8位的
          bits<1> ra;   //对应A, A是1位的


          let Opcode = op;
          let Inst{9-2} = rf;
          let Inst{1-1} = ra;
          }

// Format C instruction class in SDIC : <|opcode|F|B|A|>
// 这是F B A类型的指令
 class FC<bits<6> op, dag outs, dag ins, string asmstr,
                  list<dag> pattern, InstrItinClass itin>:
                  SDICInst<outs, ins, asmstr, pattern, itin, FrmC>
         {
          bits<8> rf;   //对应F，F是8位的
          bits<1> ra;   //对应A, A是1位的

          let Opcode = op;
          let Inst{9-2} = rf;
          let Inst{1-1}   = ra;
          }

// Format D instruction class in SDIC : <|opcode|K|>
// 这是 K 类型的指令,K是立即数 0<=K<=255

// Format E instruction class in SDIC : <|opcode|N|>
// 这是 N 类型的的指令，N应该也是个立即数（？） -1024<=N<=1023
//跳转之类的语句
class FE<bits<6> op, dag outs, dag ins, string asmstr,
                 list<dag> pattern, InstrItinClass itin>:
                 SDICInst<outs, ins, asmstr, pattern, itin, FrmE>
              {
                 bits<10> rn;

                 let Opcode=op;
                 let Inst{9-0} =rn;
               }


// Format F instruction class in SDIC : <|opcode|N|{,S}|>
// 这是 NS 类型的指令，N的取值为 0<=N<=1048575, S为1位

// Format G instruction class in SDIC : <|opcode|>
// 这是无操作数的指令

// Format H instruction class in SDIC : <|opcode|S|>
// 这是只有一个一位的操作数的指令 S为1位的

class F_K<bits<6>op, dag outs, dag ins, string asmstr,
              list<dag> pattern, InstrItinClass itin>:
                    SDICInst<outs, ins, asmstr, pattern, itin, FrmG>
                    {
                        bits<8> rk;
                        bits<2> shamt;

                        let Opcode = op;
                        let Inst{9-2} = rk;
                        let Inst{1-0} = shamt;            
                    }




class FF<bits<6> op, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>:
                   SDICInst<outs, ins, asmstr, pattern, itin, FrmF>
                   {
                        bits<2>  ra;
                        bits<2>  rb;
                        bits<2>  rc;
                        bits<4> shamt;

                        let Opcode = op;
                        let Inst{9-8} = ra;
                        let Inst{7-6} = rb;
                        let Inst{5-4} = rc;
                        let Inst{3-0}  = shamt;
                   }

class FG<bits<6> op, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>:
                   SDICInst<outs, ins, asmstr, pattern, itin, FrmG>
                   {
                        bits<2>  ra;
                       // bits<2>  rb;
                       // bits<2>  rc;
                        bits<8> shamt;

                        let Opcode = op;
                        let Inst{9-8} = ra;
                       // let Inst{7-6} = rb;
                       // let Inst{5-4} = rc;
                        let Inst{7-0}  = shamt;
                   }





















