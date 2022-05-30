#pragma once

#include <cstdio>
#include <iostream>

// Native Functions in C-Minus VM
class NativeFunc
{
public:
    template <typename T>
    static T input()
    {
        T ret;
        std::cin >> ret;
        return ret;
    }

    template <typename T>
    static void output(T t)
    {
        std::cout << t << "\n";
    }
};
