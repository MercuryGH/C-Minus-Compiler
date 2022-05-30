#pragma once

#include <unordered_map>
#include <string>

enum class InstructionType
{
    // ALU
    ADD,
    SUB,
    MUL,
    DIV,

    // CMP
    LT,
    LTE,
    GT,
    GTE,
    EQ,
    NEQ,

    // LOAD CONSTANT
    LDC,
    // LOAD
    LD,
    ABSLD,
    // STORE
    ST,
    ABSST,

    // STACK
    PUSH,
    POP,

    // BRANCH
    JMP,
    JZ,
    // SUBPROGRAM
    CALL,
    RET,
    // fetch array address
    ADDR,

    // IO (Call VM Native Method)
    IN,
    OUT,
};

// 0 operand string to InstructionType
const std::unordered_map<std::string, InstructionType> NULLARY_INST_STR2TYPE{
    {"add", InstructionType::ADD},
    {"sub", InstructionType::SUB},
    {"mul", InstructionType::MUL},
    {"div", InstructionType::DIV},

    {"lt", InstructionType::LT},
    {"lte", InstructionType::LTE},
    {"gt", InstructionType::GT},
    {"gte", InstructionType::GTE},
    {"eq", InstructionType::EQ},
    {"neq", InstructionType::NEQ},

    {"ld", InstructionType::LD},
    {"absld", InstructionType::ABSLD},
    {"st", InstructionType::ST},
    {"absst", InstructionType::ABSST},

    {"push", InstructionType::PUSH},
    {"pop", InstructionType::POP},

    {"ret", InstructionType::RET},

    {"in", InstructionType::IN},
    {"out", InstructionType::OUT},
};

// 1 operand
const std::unordered_map<std::string, InstructionType> UNARY_INST_STR2TYPE{
    {"ldc", InstructionType::LDC},

    {"jmp", InstructionType::JMP},
    {"jz", InstructionType::JZ},
    {"call", InstructionType::CALL},

    {"addr", InstructionType::ADDR},
};


// const std::unordered_map<InstructionType, std::string> UNARY_INST_TYPE2STR{
//     {InstructionType::LDC, "ldc"},

//     {InstructionType::JMP, "jmp"},
//     {InstructionType::JZ, "jz"},
//     {InstructionType::CALL, "call"},

//     {InstructionType::ADDR, "addr"},
// };