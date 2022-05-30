#include "Compiler.h"

int main(int argc, char **argv)
{
    Compiler compiler;
    bool flag = compiler.readArgs(argc, argv);
    if (flag) {
        compiler.compile();
    }

    return 0;
}