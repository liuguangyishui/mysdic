
set(LLVM_TARGET_DEFINITIONS SDICOther.td)

# Generate SDICGenRegisterInfo.inc and SDICGenInstrInfo.inc which included by 
#  your hand code C++ files. 
# SDICGenRegisterInfo.inc came from SDICRegisterInfo.td, SDICGenInstrInfo.inc 
#  came from SDICInstrInfo.td.
tablegen(LLVM SDICGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM SDICGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM SDICGenCodeEmitter.inc -gen-emitter)
tablegen(LLVM SDICGenMCCodeEmitter.inc -gen-emitter)

tablegen(LLVM SDICGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM SDICGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM SDICGenCallingConv.inc -gen-callingconv)
tablegen(LLVM SDICGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM SDICGenMCPseudoLowering.inc -gen-pseudo-lowering)

# SDICCommonTableGen must be defined
add_public_tablegen_target(SDICCommonTableGen)

# SDICCodeGen should match with LLVMBuild.txt SDICCodeGen
add_llvm_target(SDICCodeGen
  SDICAnalyzeImmediate.cpp
  SDICAsmPrinter.cpp
  SDICMCInstLower.cpp
  SDICFrameLowering.cpp
  SDICInstrInfo.cpp
  SDICISelLowering.cpp
  SDICMachineFunction.cpp
  SDICRegisterInfo.cpp
  SDICSEFrameLowering.cpp
  SDICSEInstrInfo.cpp
  SDICSEISelLowering.cpp
  SDICSERegisterInfo.cpp
  SDICSubtarget.cpp
  SDICTargetObjectFile.cpp
  SDICISelDAGToDAG.cpp
  SDICSEISelDAGToDAG.cpp
  SDICTargetMachine.cpp
  )

# Should match with "subdirectories =  MCTargetDesc TargetInfo" in LLVMBuild.txt
add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)

