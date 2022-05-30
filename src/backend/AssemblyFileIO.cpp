#include "AssemblyFileIO.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <boost/format.hpp>

void AssemblyFileIO::throwInvalidInstErr(const string &token)
{
    throw std::runtime_error((
        boost::format("Invalid token %s found") % token)
    .str());
}

// C-Minus ASM to array of VMInst
vector<VMInst> AssemblyFileIO::readAsmFile(const string &asmFilePath)
{
    vector<VMInst> res;

    vector<string> tokens = getVMInstTokens(asmFilePath);

    for (auto itr = tokens.begin(); itr != tokens.end(); itr++) {
        const string &token = *itr;

        if (NULLARY_INST_STR2TYPE.find(token) != NULLARY_INST_STR2TYPE.end()) {
            res.emplace_back(NULLARY_INST_STR2TYPE.at(token));
        } else if (UNARY_INST_STR2TYPE.find(token) != UNARY_INST_STR2TYPE.end()) {
            if (itr + 1 != tokens.end()) {
                const int operand = std::stoi(*(itr + 1));
                res.emplace_back(UNARY_INST_STR2TYPE.at(token), operand);
                itr++;
            } else {
                // throw err
                throwInvalidInstErr(*itr);
            }
        } else {
            throwInvalidInstErr(*itr);
        }
    }

    return res;
}

// array of Attribute Instruction to C-Minus ASM
void AssemblyFileIO::writeAsmFile(const string &asmFilePath, const vector<Instruction> &insts)
{
    std::ofstream writeFile(asmFilePath);

    for (const auto &inst : insts) {
        const string &opcode = inst.opcodeStr;
        if (NULLARY_INST_STR2TYPE.find(opcode) != NULLARY_INST_STR2TYPE.end()) {
            writeFile << opcode << "\n";
        } else if (UNARY_INST_STR2TYPE.find(opcode) != UNARY_INST_STR2TYPE.end()) {
            const string &operand = inst.operandStr;
            writeFile << opcode << " " << operand << "\n";
        } else {
            throwInvalidInstErr(opcode);
        }
    }
}

vector<string> AssemblyFileIO::getVMInstTokens(const string &asmFilePath) 
{
    std::ifstream readFile(asmFilePath);
    string line;
    vector<string> tokens;

    while (std::getline(readFile, line)) {
        std::istringstream readLine(line);
        string token;

        // split by space
        while (std::getline(readLine, token, ' ')) {
            tokens.push_back(token);
        }
    }

    return tokens;
}