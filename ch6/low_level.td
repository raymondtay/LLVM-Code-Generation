// ❯ llvm-tblgen ./low_level.td -print-records
// ------------- Classes -----------------
// class Instruction<string Instruction:mnemonic = ?, int Instruction:opcode = ?> {
//   string Mnemonic = Instruction:mnemonic;
//   int Opcode = Instruction:opcode;
// }
// ------------- Defs -----------------
// def ADD {	// Instruction
//   string Mnemonic = "add";
//   int Opcode = 2;
// }
// def MOV {	// Instruction
//   string Mnemonic = "mov";
//   int Opcode = 1;
// }
//
// -------------------***********-----------------
//
// ❯ llvm-tblgen -print-records ./low_level.td
// ------------- Classes -----------------
// class Instruction<string Instruction:mnemonic = ?, int Instruction:opcode = ?> {
//   string Mnemonic = Instruction:mnemonic;
//   int Opcode = Instruction:opcode;
// }
// class Register<string Register:name = ?, int Register:num = ?> {
//   string Name = Register:name;
//   int Num = Register:num;
// }
// class RegisterClass<list<Register> RegisterClass:regs = ?> {
//   list<Register> Regs = RegisterClass:regs;
// }
// ------------- Defs -----------------
// def ADD {	// Instruction
//   string Mnemonic = "add";
//   int Opcode = 2;
// }
// def GPR {	// RegisterClass
//   list<Register> Regs = [R0, R1, R2];
// }
// def MOV {	// Instruction
//   string Mnemonic = "mov";
//   int Opcode = 1;
// }
// def R0 {	// Register
//   string Name = "r0";
//   int Num = 0;
// }
// def R1 {	// Register
//   string Name = "r1";
//   int Num = 1;
// }
// Odef R2 {	// Register
//   string Name = "r2";
//   int Num = 2;
// }
//
// def MyRecord {
//   list<int> ZeroTo10No5 = [0, 1, 2, 3, 4, 6, 7, 8, 9];
//   int num = 9;
// }

class Instruction<string mnemonic, int opcode> {
    string Mnemonic = mnemonic;
    int Opcode = opcode;
}

def MOV : Instruction<"mov", 0x01>;
def ADD : Instruction<"add", 0x02>;

class Register<string name, int num> {
    string Name = name;
    int Num = num;
}

def R0 : Register<"r0", 0>;
def R1 : Register<"r1", 1>;
def R2 : Register<"r2", 2>;

class RegisterClass<list<Register> regs> {
    list<Register> Regs = regs;
}

def GPR : RegisterClass<[R0, R1, R2]>;

def MyRecord {
  list<int> ZeroTo10No5 =
      !filter(var, !range(0, 10), !ne(var, 5));
  int num = !size(ZeroTo10No5);
}



