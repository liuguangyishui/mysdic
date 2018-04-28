//===-- SDICSEISelDAGToDAG.h - A Dag to Dag Inst Selector for SDICSE -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of SDICDAGToDAGISel specialized for cpu032.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_SDIC_SDICSEISELDAGTODAG_H
#define LLVM_LIB_TARGET_SDIC_SDICSEISELDAGTODAG_H

#include "SDICConfig.h"

#include "SDICISelDAGToDAG.h"

namespace llvm {

class SDICSEDAGToDAGISel : public SDICDAGToDAGISel {

public:
  explicit SDICSEDAGToDAGISel(SDICTargetMachine &TM, CodeGenOpt::Level OL)
      : SDICDAGToDAGISel(TM, OL) {}

private:

  bool runOnMachineFunction(MachineFunction &MF) override;

  bool trySelect(SDNode *Node) override;

  void processFunctionAfterISel(MachineFunction &MF) override;

  // Insert instructions to initialize the global base register in the
  // first MBB of the function.
//  void initGlobalBaseReg(MachineFunction &MF);

};

FunctionPass *createSDICSEISelDag(SDICTargetMachine &TM,
                                  CodeGenOpt::Level OptLevel);

}

#endif

