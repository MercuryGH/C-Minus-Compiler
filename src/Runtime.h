#pragma once

#include <string>

using std::string;

class Runtime
{
public:
    Runtime() {}

    bool readArgs(int argc, char **argv);
    void execCode() const;
    
private:
    string asmFilePath;

    const static string WELCOME_PROMPT;
};
