//===-- SDICInstPrinter.cpp - Convert SDIC MCInst to assembly syntax ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an SDIC MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "SDICInstPrinter.h"

#include "SDICInstrInfo.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"


using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#define PRINT_ALIAS_INSTR
#include "SDICGenAsmWriter.inc"

//covert the decimal number to hex

std::string covert(int64_t Imm) {

  std::string signal[16] = {"0", "1", "2", "3",
		       "4", "5", "6", "7",
		       "8", "9", "A", "B",
		       "C", "D", "E", "F"};
  //272是由256 + 16 而得到的
  //256是第一个Bank区，
  //64则是代表第二个Bank的前64个已经用作通用寄存器了
  int64_t value = Imm + 272;
  std::string res;
  if(0 <= value && value < 16) {
    res = signal[value];
    return res;
  }
  else {
    
    int sh = value, yu;
    bool index = true;
    while(index) {

      yu = sh % 16;
      sh = sh / 16; 
      if(sh >= 16) {
	res =  signal[yu] + res;
      }
      else {
	res = signal[sh] + signal[yu] + res;
	index = false;
      }
    }
  }
  return res; 
}

void SDICInstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const {
//- getRegisterName(RegNo) defined in SDICGenAsmWriter.inc which indicate in 
//   SDIC.td.'$'
  //OS << '$' << StringRef(getRegisterName(RegNo)).lower();
  OS << StringRef(getRegisterName(RegNo));
}

//@1 {
void SDICInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                StringRef Annot, const MCSubtargetInfo &STI) {
  // Try to print any aliases first.
  if (!printAliasInstr(MI, O))
//@1 }
    //- printInstruction(MI, O) defined in SDICGenAsmWriter.inc which came from 
    //   SDIC.td indicate.
  
 
  //   unsigned Opcode_var =  MI->getOpcode();
    
  //   StringRef OpcodeName = getOpcodeName(Opcode_var);
  // O << StringRef("there is a tag");
  

    if(getOpcodeName(MI->getOpcode())=="ADDiua")
      O << StringRef(getOpcodeName(MI->getOpcode()));
     
   
   printInstruction(MI, O);
   printAnnotation(O, Annot);
}

// 打印操作数的同时还要根据其地址决定是否是Bank1还是Bank2的地址
void SDICInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  llvm::StringRef InstName = getOpcodeName(MI->getOpcode());

  //* 对于Load和Store指令操作数的处理
  if(InstName =="LD"||  InstName == "ST") {
    //** 操作数为寄存器
    if(Op.isReg()) {
	printRegName(O, Op.getReg());
	  // O << "\t" << (Op.getReg() >15)? 1: 0);
	if(Op.getReg() >= 10 && Op.getReg() < 26)  O << "，\t" << 1;
	else                                       O << "，\t" << 0;
	//O << StringRef(getOpcodeName(MI->getOpcode()));
	return;
      }
    //** 操作数为绝对地址
    if(Op.isImm()) {
      std::string Imm = covert(Op.getImm());
      if(InstName == "LD"){
	//	O << Imm << "H" << "\t" << 1 << "\t" << 1;
	O << Op.getImm() << "H," << "\t" << 1 << "," << "\t" << 1;
      }
      else if(InstName == "ST") {
	//	O << Imm << "H" << "\t" << 1;
	O << Op.getImm() << "H," << "\t" << 1;
      }
      return;
    }
  }
  //* 对其他的指令进行操作数的处理
  if (Op.isReg()) {
    //** 打印了指令中的寄存器，这个寄存器是对A参数进行判断的依据; 跟据寄存器的类型决定A的值，A代表操作区
    //** [10, 25]是通用寄存器的范围,如果寄存器在这个范围内则是Bank2,则A为1
    printRegName(O, Op.getReg());
    //** 指令类型有多种，如F,D,A; F,A; F,B,A
    //** 因为F,B,A类型中的B在别处处理，所以这里可以把后两种指令合并处理
    if(InstName == "CPFSEQ" || InstName == "Movlw" ||    //Movlw是对于内部的Movwf而定义的
       InstName == "Movwf"  || InstName == "ADDLW" ||    //ADDLW也是针对于内部的Movwf而定义的
       InstName == "BTFSS"  || InstName == "BTFSC" ||
       InstName == "BCF" )   {
      if(Op.getReg() >= 10 && Op.getReg() < 26)
	O << ",\t" << 1;
      else
	O << ",\t" << 0;
    }
    //** 这里主要解决F,D,A指令，两个参数，D默认为0
    else {
      if(Op.getReg()>= 10 && Op.getReg() < 26 )
	O << ",\t" << 0 << ",\t" << 1; 
      else
	O << ",\t" << 0 << ",\t" << 0;
    }
    
    return;
  }
  if (Op.isImm()) {
    O << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  Op.getExpr()->print(O, &MAI, true);
}



void SDICInstPrinter::printUnsignedImm(const MCInst *MI, int opNum,
                                       raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(opNum);
  if (MO.isImm())
    O << (unsigned short int)MO.getImm();
  else
    printOperand(MI, opNum, O);
}

void SDICInstPrinter::
printMemOperand(const MCInst *MI, int opNum, raw_ostream &O) {
  // Load/Store memory operands -- imm($reg)
  // If PIC target the target is loaded as the
  // pattern ld $t9,%call16($gp)
  printOperand(MI, opNum+1, O);
  //HYL 删除了打印栈指针的内容
  /*
  O << "(";
  printOperand(MI, opNum, O);
  O << ")";
  */
}

//#if CH >= CH7_1
// The DAG data node, mem_ea of SDICInstrInfo.td, cannot be disabled by
// ch7_1, only opcode node can be disabled.
void SDICInstPrinter::
printMemOperandEA(const MCInst *MI, int opNum, raw_ostream &O) {
  // when using stack locations for not load/store instructions
  // print the same way as all normal 3 operand instructions.

  //HYL printOperand(MI, opNum, O);
  //HYL O << ", ";
  printOperand(MI, opNum+1, O);
  O << "H"; //HYL
  return;
}
//#endif

