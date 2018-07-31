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
  //加64是代表前64位已经用作通用寄存器了
  int64_t value = Imm + 64;
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
  OS << StringRef(getRegisterName(RegNo)).lower();
}

//@1 {
void SDICInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                StringRef Annot, const MCSubtargetInfo &STI) {
  // Try to print any aliases first.
  if (!printAliasInstr(MI, O))
//@1 }
    //- printInstruction(MI, O) defined in SDICGenAsmWriter.inc which came from 
    //   SDIC.td indicate.
    printf("there is a tag");
    printf("the MI->getOpcode() is %u", MI->getOpcode());
  //   unsigned Opcode_var =  MI->getOpcode();
    
  //   StringRef OpcodeName = getOpcodeName(Opcode_var);
  // O << StringRef("there is a tag");
  

    if(getOpcodeName(MI->getOpcode())=="ADDiua")
      O << StringRef(getOpcodeName(MI->getOpcode()));
     
   
   printInstruction(MI, O);
   printAnnotation(O, Annot);
}

void SDICInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);

  //comfirm the InstName
  if(getOpcodeName(MI->getOpcode())=="LD"||getOpcodeName(MI ->getOpcode()) == "ST") {
      if(Op.isReg()) {
	/* if(Op.getReg() == 20) {
	      return;
	      }*/
	  printRegName(O, Op.getReg());
	  O << "\t" << (Op.getReg() >15)? 1: 0);
      /* if(Op.getReg() > 15) O << "\t" << 1;
	 else               O << "\t" << 0;*/
	//O << StringRef(getOpcodeName(MI->getOpcode()));
	return;
      }
      if(Op.isImm()) {
	std::string Imm = covert(Op.getImm());
	O << Imm <<　"H" << "\t" << 1 << "\t" << 1;
	return;
      }
  }     
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    //跟据寄存器的类型决定A的值，A代表操作区
    if(Op.getReg() > 15)  O << "\t" << 1; 
    else                O << "\t" << 0;
    
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
  printOperand(MI, opNum, O);
  O << ", ";
  printOperand(MI, opNum+1, O);
  return;
}
//#endif

