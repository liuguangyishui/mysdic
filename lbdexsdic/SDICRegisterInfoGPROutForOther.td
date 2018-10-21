
//===----------------------------------------------------------------------===//
// Register Classes
//===----------------------------------------------------------------------===//

def GPROut : RegisterClass<"SDIC", [i32], 32, (add (sub SDICRegs, STATUS))>;


def OutWreg : RegisterClass<"SDIC", [i32], 32, (add SDICWreg)>;

//def GPROut : RegisterClass<"SDIC", [i32], 32, (add (sub SDICReservedReg, R3, R8))>;

