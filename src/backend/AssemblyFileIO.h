#pragma once

#include <vector>
#include "VMInst.h"      // VM Instruction
#include "Instruction.h" // Attribute Instruction

using std::vector;

class AssemblyFileIO {
public:
    // Decoder
    static vector<VMInst> readAsmFile(const string &asmFilePath);

    // Encoder
    static void writeAsmFile(const string &asmFilePath, const vector<Instruction> &insts);
private:
    static void throwInvalidInstErr(const string &token);

    static vector<string> getVMInstTokens(const string &asmFilePath);
};