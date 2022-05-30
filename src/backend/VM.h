#pragma once

#include <vector>
#include <string>
#include "VMInst.h"

using std::vector;
using std::string;
using std::pair;

class VM
{
public:
    VM(const vector<VMInst> &codes): codes(codes) {}

    void run();
private:
    // memory
    vector<VMInst> codes;
    vector<int> stack;

    // registers
    int pc;   // Program Counter
    int acc;  // Accumulator (GPR)
    int base; // Stack Frame Pointer
    // int sp;// Stack Pointer (in VM, this can be achieved by stack.size())

    void exec(const VMInst &instruction);
};

/*
https://en.wikipedia.org/wiki/Stack_machine
https://en.wikipedia.org/wiki/Accumulator_(computing)

C-Minus VM is a hybrid, turing-complete VM (Stack Machine and Accumulator Machine).

Calling Convention: 
1. Passing Variable by stack, from right to left.
2. Push local variables first, then set base register to new stack frame.
*/

/*
| Instruction |                    Pseudocode                    |
| :---------: | :----------------------------------------------: |
|     add     |              acc = stack.top + acc               |
|     sub     |              acc = stack.top - acc               |
|     mul     |              acc = stack.top * acc               |
|     div     |              acc = stack.top / acc               |
|     lt      |              acc = stack.top < acc               |
|     lte     |              acc = stack.top <= acc              |
|     gt      |              acc = stack.top > acc               |
|     gte     |              acc = stack.top >= acc              |
|     eq      |              acc = stack.top == acc              |
|     neq     |              acc = stack.top != acc              |
|    ldc n    |                     acc = n                      |
|     ld      |                acc = [base - acc]                |
|    absld    |                   acc = [acc]                    |
|     st      |             [base - acc] = stack.top             |
|    absst    |                [acc] = stack.top                 |
|    push     |                 stack.push(acc)                  |
|     pop     |                   stack.pop()                    |
|    jmp n    |                     pc += n                      |
|    jz n     |              if (acc == 0) pc += n               |
|   call n    |    push base; base = sp - 2; push pc; pc += n    |
|     ret     |                 pop pc; pop base                 |
|   addr n    |                   acc = sp - n                   |
|     in      |      (Native Function Call) std::cin >> acc      |
|     out     | (Native Function Call)  std::cout << acc << "\n" |

ld / st: locate local variable
absld / absst: locate gloabl variable, and array entry (ARRAY_BASE + OFFSET)
call: In assembler, only `call LABEL` is generated. 
      Linker will translate it to `call n` later.
      `call` also contains the prologue of function. 
      `ret` contains the epilogue of function.
*/