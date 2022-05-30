#include <stdexcept>
#include "VM.h"
#include "NativeFunc.h"

void VM::run()
{
    for (pc = 0; pc < codes.size(); pc++)
    {
        exec(codes[pc]);
    }
}

void VM::exec(const VMInst &instruction)
{
    switch (instruction.opcode)
    {
    case InstructionType::ADD:
        acc = stack.back() + acc;
        break;

    case InstructionType::SUB:
        acc = stack.back() - acc;
        break;

    case InstructionType::MUL:
        acc = stack.back() * acc;
        break;

    case InstructionType::DIV:
        acc = stack.back() / acc;
        break;

    case InstructionType::LT:
        acc = stack.back() < acc;
        break;

    case InstructionType::LTE:
        acc = stack.back() <= acc;
        break;

    case InstructionType::GT:
        acc = stack.back() > acc;
        break;

    case InstructionType::GTE:
        acc = stack.back() >= acc;
        break;

    case InstructionType::EQ:
        acc = stack.back() == acc;
        break;

    case InstructionType::NEQ:
        acc = stack.back() != acc;
        break;

    case InstructionType::LDC:
        acc = instruction.operand;
        break;

    case InstructionType::LD:
        acc = stack[base - acc];
        break;

    case InstructionType::ABSLD:
        acc = stack[acc];
        break;

    case InstructionType::ST:
        stack[base - acc] = stack.back();
        break;

    case InstructionType::ABSST:
        stack[acc] = stack.back();
        break;

    case InstructionType::PUSH:
        stack.push_back(acc);
        break;

    case InstructionType::POP:
        stack.pop_back();
        break;

    case InstructionType::JMP:
        // -1 is to dealing with pc++ in VM::run()
        pc += instruction.operand - 1;
        break;

    case InstructionType::JZ:
        if (acc == 0)
        {
            pc += instruction.operand - 1;
        }
        break;

    case InstructionType::CALL:
        // Locals ... Params

        stack.push_back(base);
        // Locals ... Params OLD_BASE

        base = stack.size() - 2;
        // base = &(LAST_PARAM)

        stack.push_back(pc);
        // Locals ... Params OLD_BASE OLD_PC

        pc += instruction.operand - 1;
        break;

    case InstructionType::RET:
        pc = stack.back();
        stack.pop_back();
        base = stack.back();
        stack.pop_back();
        break;

    case InstructionType::ADDR:
        acc = stack.size() - instruction.operand;
        break;

    case InstructionType::IN:
        // scanf("%d", &acc);
        acc = NativeFunc::input<int>();
        break;

    case InstructionType::OUT:
        // printf("%d\n", acc);
        NativeFunc::output(acc);
        break;

    default:
        throw std::runtime_error("Invalid Instruction!");
    }
}