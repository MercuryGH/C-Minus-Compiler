#pragma once

#include <string>

using std::string;

const string GLOBAL_SCOPE_NAME = "$global";
const string MAIN_NAME = "main";

class Compiler
{
public:
    Compiler() {}

    bool readArgs(int argc, char **argv);
    void compile() const;

private:
    string srcFilePath;
    string outputFilePath;
    string asmFilePath;
    string visualizeAstFilePath;

    const static string WELCOME_PROMPT;
};
