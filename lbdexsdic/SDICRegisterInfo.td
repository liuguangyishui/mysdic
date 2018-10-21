//===-- SDICRegisterInfo.td - SDIC Register defs -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//



//=====----------------------------------------------------------------===//
//   Declarations that describe the SDIC register file
//=====----------------------------------------------------------------===//

// We have banks of 8 registers each.
class SDICReg<bits<16> Enc, string n> : Register<n> {
      let HWEncoding =Enc;
      let Namespace ="SDIC";
      }

//SDIC General Purpose Register
class SDICGPRReg<bits<16> Enc, string n> : SDICReg<Enc, n>;

//SDIC Special Register
//定义的是间接寻址寄存器
class SDICFSRReg<bits<16> Enc, string n> : SDICReg<Enc, n>;

//定义的是程序计数器和栈顶寄存器
class SDICPTReg<bits<16> Enc, string n> : SDICReg<Enc, n>;

//===------------------------------------------------------------------===//
// @Registers
//===------------------------------------------------------------------===//
// @All registers definition
let Namespace = "SDIC" in {
    //@ General Purpose Registers

    //循环定义256个通用寄存器
       /* foreach i=0-15 in {
                def R#i : SDICGPRReg<i, "r"#i>, DwarfRegNum<[i]>;
                }*/
                // 16个虚拟寄存器
              
                def R0   : SDICGPRReg<0,  "R0">, DwarfRegNum<[0]>; //100H
                def R1   : SDICGPRReg<1,  "R1">, DwarfRegNum<[1]>; //101H
                def R2   : SDICGPRReg<2,  "R2">, DwarfRegNum<[2]>;
                def R3   : SDICGPRReg<3,  "R3">, DwarfRegNum<[3]>;
                def R4   : SDICGPRReg<4,  "R4">, DwarfRegNum<[4]>;
                def R5   : SDICGPRReg<5,  "R5">, DwarfRegNum<[5]>;
                def R6   : SDICGPRReg<6,  "R6">, DwarfRegNum<[6]>;
                def R7   : SDICGPRReg<7,  "R7">, DwarfRegNum<[7]>;
                def R8   : SDICGPRReg<8,  "R8">, DwarfRegNum<[8]>;
                def R9   : SDICGPRReg<9,  "R9">, DwarfRegNum<[9]>;
                def R10  : SDICGPRReg<10, "R10">, DwarfRegNum<[10]>;
                def R11  : SDICGPRReg<11, "R11">, DwarfRegNum<[11]>;
                def R12  : SDICGPRReg<12, "R12">, DwarfRegNum<[12]>;
                def R13  : SDICGPRReg<13, "R13">, DwarfRegNum<[13]>;
                def R14  : SDICGPRReg<14, "R14">, DwarfRegNum<[14]>;
                def R15  : SDICGPRReg<15, "R15">, DwarfRegNum<[15]>;//10FH
                //临时寄存器
                def Temp_Reg :SDICGPRReg<16, "temp_reg">, DwarfRegNum<[16]>;
                //工作寄存器
                def WREG : SDICGPRReg<17, "wreg">, DwarfRegNum<[17]>;
                     //状态指示寄存器
                def SS : SDICGPRReg<18, "status">, DwarfRegNum<[18]>;
                def STATUS : SDICGPRReg<28, "status">, DwarfRegNum<[28]>;
                //间接寻址寄存器
                def FSR0L : SDICGPRReg<19, "fsr0l">, DwarfRegNum<[19]>;
                def FSR0H : SDICGPRReg<20, "fsr0h">, DwarfRegNum<[20]>;
                def FSR2L : SDICGPRReg<21, "fsr2l">, DwarfRegNum<[21]>;
                def FSR2H : SDICGPRReg<22, "fsr2h">, DwarfRegNum<[22]>;

                //堆栈栈顶寄存器
                def TOSL :  SDICPTReg<0, "tosl">, DwarfRegNum<[268]>;
                def TOSH :  SDICPTReg<1, "tosh">, DwarfRegNum<[269]>;
                //PC寄存器
                def PCL  :  SDICPTReg<2, "pcl">,  DwarfRegNum<[270]>;
                def PCH  :  SDICPTReg<3, "pch">,  DwarfRegNum<[267]>;
             
                
                 //堆栈指示寄存器
                def STKPTR : SDICReg<2, "stkptr">, DwarfRegNum<[262]>;
}

//@Register Classes
//===----------------------------------------------------------------===//
def SDICRegs : RegisterClass<"SDIC", [i32], 32, (add
    R0,  R1,  R2,  R3,
    R4,  R5,  R6,  R7,
    R8,  R9,  R10, R11,
    R12, R13, R14, R15,
    STKPTR, FSR0L, FSR0H,
    FSR2L,  FSR2H
   // TOSL, TOSH, PCL, PCH
   )>;

//@Status Register Class
def SR : RegisterClass<"SDIC", [i32], 32, (add SS)>;//, R6,R7, R10, R11, R12)>;

//@Co-processor 0 Registers Class
def C0Regs : RegisterClass<"SDIC", [i32], 32, (add PCL)>;
def SDICReservedReg : RegisterClass<"SDIC", [i32], 32, (add R2)>;
def SDICWreg : RegisterClass<"SDIC", [i32], 32, (add R1)>;
