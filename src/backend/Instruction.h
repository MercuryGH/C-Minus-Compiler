#pragma once

#include <string>

using std::string;

struct Instruction {
    string opcodeStr;
    string operandStr;

    Instruction(string opcodeStr):
        opcodeStr(opcodeStr), operandStr("") {}

    Instruction(string opcodeStr, string operandStr):
        opcodeStr(opcodeStr), operandStr(operandStr) {}
};