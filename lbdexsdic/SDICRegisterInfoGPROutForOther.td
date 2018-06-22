
//===----------------------------------------------------------------------===//
// Register Classes
//===----------------------------------------------------------------------===//

def GPROut : RegisterClass<"SDIC", [i32], 32, (add (sub SDICRegs, STATUS))>;

def GPROuttwo : RegisterClass<"SDIC", [i32], 32, (add (sub SDICReservedReg))>;

