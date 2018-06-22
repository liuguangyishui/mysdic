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
        foreach i=0-15 in {
                def R#i : SDICGPRReg<i, "r"#i>, DwarfRegNum<[i]>;
                }
                def WREG : SDICReg<0, "wreg">, DwarfRegNum<[260]>;
                //状态指示寄存器
                def STATUS : SDICReg<1, "status">, DwarfRegNum<[261]>;
                //堆栈指示寄存器
                def STKPTR : SDICReg<2, "stkptr">, DwarfRegNum<[262]>;
                //间接寻址寄存器
                def FSR0L : SDICFSRReg<0, "fsr0l">, DwarfRegNum<[263]>;
                def FSR0H : SDICFSRReg<1, "fsr0h">, DwarfRegNum<[264]>;
                def FSR2L : SDICFSRReg<2, "fsr2l">, DwarfRegNum<[265]>;
                def FSR2H : SDICFSRReg<3, "fsr2h">, DwarfRegNum<[267]>;
                //堆栈栈顶寄存器
                def TOSL :  SDICPTReg<0, "tosl">, DwarfRegNum<[268]>;
                def TOSH : SDICPTReg<1, "tosh">,DwarfRegNum<[269]>;
                //PC寄存器
                def PCL : SDICPTReg<2, "pcl">, DwarfRegNum<[270]>;
                def PCH : SDICPTReg<3, "pch">, DwarfRegNum<[271]>;
}

//@Register Classes
//===----------------------------------------------------------------===//
def SDICRegs : RegisterClass<"SDIC", [i32], 32, (add
   // R0,  R1,  R2,  R3,
    R4,  R5,  R6,  R7,
    R8,  R9,  R10, R11,
    R12, R13, R14, R15,
    STATUS, STKPTR, FSR0L, FSR2L, TOSL )>;

//@Status Register Class
def SR : RegisterClass<"SDIC", [i32], 32, (add STATUS)>;

//@Co-processor 0 Registers Class
def C0Regs : RegisterClass<"SDIC", [i32], 32, (add PCL)>;
def SDICReservedReg : RegisterClass<"SDIC", [i32], 32, (add R2)>;

