#include "Runtime.h"

int main(int argc, char **argv)
{
    Runtime runtime;
    bool flag = runtime.readArgs(argc, argv);
    if (flag) {
        runtime.execCode();
    }

    return 0;
}