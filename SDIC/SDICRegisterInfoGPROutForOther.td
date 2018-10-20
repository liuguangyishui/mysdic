
//===----------------------------------------------------------------------===//
// Register Classes
//===----------------------------------------------------------------------===//

def GPROut : RegisterClass<"SDIC", [i32], 32, (add (sub SDICRegs, STATUS))>;

def GPROut_test : RegisterClass<"SDIC", [i32], 32, (add R3)>;

//def GPROut : RegisterClass<"SDIC", [i32], 32, (add (sub SDICReservedReg, R3, R8))>;

