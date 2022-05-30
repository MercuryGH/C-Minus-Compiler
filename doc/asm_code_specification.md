# C Minus Assembly Code Specification

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
