//===SDIC-- SDICSEInstrInfo.cpp - SDIC32/64 Instruction Information -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the SDIC32/64 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "SDICSEInstrInfo.h"

#include "InstPrinter/SDICInstPrinter.h"
#include "SDICMachineFunction.h"
#include "SDICTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

SDICSEInstrInfo::SDICSEInstrInfo(const SDICSubtarget &STI)
    : SDICInstrInfo(STI),
      RI(STI) {}

const SDICRegisterInfo &SDICSEInstrInfo::getRegisterInfo() const {
  return RI;
}

void SDICSEInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I,
                                  const DebugLoc &DL, unsigned DestReg,
                                  unsigned SrcReg, bool KillSrc) const {
  /*unsigned Opc = 0, ZeroReg = 0;

  if (SDIC::CPURegsRegClass.contains(DestReg)) { // Copy to CPU Reg.
    if (SDIC::CPURegsRegClass.contains(SrcReg))
      Opc = SDIC::ADDu, ZeroReg = SDIC::ZERO;
    else if (SrcReg == SDIC::HI)
      Opc = SDIC::MFHI, SrcReg = 0;
    else if (SrcReg == SDIC::LO)
      Opc = SDIC::MFLO, SrcReg = 0;
  }
  else if (SDIC::CPURegsRegClass.contains(SrcReg)) { // Copy from CPU Reg.
    if (DestReg == SDIC::HI)
      Opc = SDIC::MTHI, DestReg = 0;
    else if (DestReg == SDIC::LO)
      Opc = SDIC::MTLO, DestReg = 0;
  }

  assert(Opc && "Cannot copy registers");

  MachineInstrBuilder MIB = BuildMI(MBB, I, DL, get(Opc));

  if (DestReg)
    MIB.addReg(DestReg, RegState::Define);

  if (ZeroReg)
    MIB.addReg(ZeroReg);

  if (SrcReg)
  MIB.addReg(SrcReg, getKillRegState(KillSrc));*/
}




void SDICSEInstrInfo::
storeRegToStack(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                unsigned SrcReg, bool isKill, int FI,
                const TargetRegisterClass *RC, const TargetRegisterInfo *TRI,
                int64_t Offset) const {
  DebugLoc DL;
  MachineMemOperand *MMO = GetMemOperand(MBB, FI, MachineMemOperand::MOStore);

  unsigned Opc = 0;

  Opc = SDIC::ST;
  assert(Opc && "Register class not handled!");
  BuildMI(MBB, I, DL, get(Opc)).addReg(SrcReg, getKillRegState(isKill))
    .addFrameIndex(FI).addImm(Offset).addMemOperand(MMO);
}

void SDICSEInstrInfo::
loadRegFromStack(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                 unsigned DestReg, int FI, const TargetRegisterClass *RC,
                 const TargetRegisterInfo *TRI, int64_t Offset) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();
  MachineMemOperand *MMO = GetMemOperand(MBB, FI, MachineMemOperand::MOLoad);
  unsigned Opc = 0;

  Opc = SDIC::LD;
  assert(Opc && "Register class not handled!");
  BuildMI(MBB, I, DL, get(Opc), DestReg).addFrameIndex(FI).addImm(Offset)
    .addMemOperand(MMO);
}

//@expandPostRAPseudo
/// Expand Pseudo instructions into real backend instructions
bool SDICSEInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
  //@expandPostRAPseudo-body
  
  MachineBasicBlock &MBB = *MI.getParent();
  switch (MI.getDesc().getOpcode()) {
  default:
    return false;
  case SDIC::RetLR:
    { printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
      printf("The Opcode is %u\n", MI.getDesc().getOpcode());
      printf("The number of the Operand is %u",MI.getNumOperands());
      // cout<<MI.getDesc();
     
      //cout<<MI.getOperand(i);
	
    expandRetLR(MBB, MI);
    }
    break;
  }
  MBB.erase(MI);
  return true;
}

/// Adjust SP by Amount bytes.
void SDICSEInstrInfo::adjustStackPtr(unsigned SP, int64_t Amount,
				     MachineBasicBlock &MBB,
				     MachineBasicBlock::iterator I) const {
  DebugLoc DL = I != MBB.end() ? I->getDebugLoc() : DebugLoc();
  //unsigned ADDiua = SDIC::ADDiua;
  unsigned ADDu = SDIC::ADDu;
  unsigned ADDiu = SDIC::ADDiu;
  unsigned Movf = SDIC::Movf;
  unsigned Movwf = SDIC::Movwf;
  unsigned ADDLW = SDIC::ADDLW;
  unsigned SUBLW = SDIC::SUBLW;
  unsigned STKPTR = SDIC::STKPTR;
  unsigned PCL = SDIC::PCL;
  unsigned PCH = SDIC::PCH;
  unsigned TOSH = SDIC::TOSH;
  unsigned TOSL = SDIC::TOSL;
  if (isInt<16>(Amount)) {
    if(Amount >=0)
      {
    // addiu sp, sp, amount
    //My modify
    // BuildMI(MBB, I, DL, get(ADDiu), SP).addReg(SP).addImm(Amount);
    BuildMI(MBB, I, DL, get(Movf)).addImm(1).addReg(STKPTR);
    // BuildMI(MBB, I, DL, get(ADDLW)).addImm(1);
    BuildMI(MBB, I, DL, get(Movwf)).addImm(1).addReg(STKPTR);
    //BuildMI(MBB, I, DL, get(Movf)).addImm(1).addReg(PCL);
    // BuildMI(MBB, I, DL, get(Movwf)).addImm(1).addReg(TOSL);
    //BuildMI(MBB, I, DL, get(Movf)).addImm(1).addReg(PCH);
    // BuildMI(MBB, I, DL, get(Movwf)).addImm(1).addReg(TOSH);
    
    
    // BuildMI(MBB, I, DL, get(ADDiu), SP).addReg(SP).addImm(Amount);
     // BuildMI(MBB, I, DL, get(ADDiua), SP).addReg(SP).addImm(1).addImm(1);
      }
    uint64_t i=4;//both call fun and return fun should operate the PC register
    if(Amount<0)
      i=-i;
    //MY modify
    /*BuildMI(MBB, I, DL, get(Movf),   SDIC::TOSH);
    BuildMI(MBB, I, DL, get(Movwf),  SDIC::PCH);
    BuildMI(MBB, I, DL, get(Movf),   SDIC::TOSL);
    BuildMI(MBB, I, DL, get(Movwf),  SDIC::PCL);
    BuildMI(MBB, I, DL, get(Movf),   SDIC::STKPTR);
    BuildMI(MBB, I, DL, get(SUBLW), -1);
    BuildMI(MBB, I, DL, get(Movwf),  SDIC::STKPTR);
    */
    BuildMI(MBB,I,DL,get(ADDiu),SDIC::PCL).addReg(SDIC::PCL).addImm(i);
    //  BuildMI(MBB, I, DL, get(ADDiua), SDIC::PCL).addReg(SDIC::PCL).addImm(1).addImm(1);
    //  BuildMI(MBB,I,DL,get(SDIC::MOVF),SDIC::WREG).addReg(SDIC::PCL).addImm(i);

  }
  else {
    // Expand immediate that doesn't fit in 16-bit.
    unsigned Reg = loadImmediate(Amount, MBB, I, DL, nullptr);
    BuildMI(MBB, I, DL, get(ADDu), SP).addReg(SP).addReg(Reg, RegState::Kill);
  }

 

}

/// This function generates the sequence of instructions needed to get the
/// result of adding register REG and immediate IMM.
unsigned
SDICSEInstrInfo::loadImmediate(int64_t Imm, MachineBasicBlock &MBB,
			       MachineBasicBlock::iterator II,
			       const DebugLoc &DL,
			       unsigned *NewImm) const {
  SDICAnalyzeImmediate AnalyzeImm;
  unsigned Size = 32;
  unsigned LUi = SDIC::LUi;
  unsigned ZEROReg = SDIC::R0;
  unsigned ATReg = SDIC::R6;
  bool LastInstrIsADDiu = NewImm;
  
  const SDICAnalyzeImmediate::InstSeq &Seq =
    AnalyzeImm.Analyze(Imm, Size, LastInstrIsADDiu);
  SDICAnalyzeImmediate::InstSeq::const_iterator Inst = Seq.begin();
  assert(Seq.size() && (!LastInstrIsADDiu || (Seq.size() > 1)));
  // The first instruction can be a LUi, which is different from other
  // instructions (ADDiu, ORI and SLL) in that it does not have a register
  // operand.
  if (Inst->Opc == LUi)
    BuildMI(MBB, II, DL, get(LUi), ATReg).addImm(SignExtend64<16>(Inst->ImmOpnd));
  else
    BuildMI(MBB, II, DL, get(Inst->Opc), ATReg).addReg(ZEROReg)
      .addImm(SignExtend64<16>(Inst->ImmOpnd));
  // Build the remaining instructions in Seq.
  for (++Inst; Inst != Seq.end() - LastInstrIsADDiu; ++Inst)
    BuildMI(MBB, II, DL, get(Inst->Opc), ATReg).addReg(ATReg)
      .addImm(SignExtend64<16>(Inst->ImmOpnd));

  if (LastInstrIsADDiu)
    *NewImm = Inst->ImmOpnd;
  return ATReg;
}

  

void SDICSEInstrInfo::expandRetLR(MachineBasicBlock &MBB,
				  MachineBasicBlock::iterator I) const {
  // BuildMI(MBB, I, I->getDebugLoc(), get(SDIC::RET)).addReg(SDIC::R10);
  BuildMI(MBB, I, I->getDebugLoc(), get(SDIC::RET)).addImm(1);
}

const SDICInstrInfo *llvm::createSDICSEInstrInfo(const SDICSubtarget &STI) {
  return new SDICSEInstrInfo(STI);
}

