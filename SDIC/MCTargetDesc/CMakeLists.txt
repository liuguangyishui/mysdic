# MCTargetDesc/CMakeLists.txt
add_llvm_library(LLVMSDICDesc
  SDICABIInfo.cpp
  SDICMCAsmInfo.cpp
  SDICMCTargetDesc.cpp
  SDICMCExpr.cpp
  SDICAsmBackend.cpp
  SDICMCCodeEmitter.cpp
  SDICELFObjectWriter.cpp	
  )

