#pragma once

#include "InstructionType.h"

struct VMInst {
    InstructionType opcode;
    int operand;

    VMInst(InstructionType opcode): opcode(opcode) {}

    VMInst(InstructionType opcode, int operand): 
        opcode(opcode), operand(operand) {}
};