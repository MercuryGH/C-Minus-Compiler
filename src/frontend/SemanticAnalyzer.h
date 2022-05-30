#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include "VariableAttribute.h"
#include "AST.h"

// { stirng: { string: VariableAttribute } }
// { ScopeName: { VariableName: VariableAttribute } }
using SymbolTable = std::unordered_map<string, std::unordered_map<string, VariableAttribute>>;

// 未做语义检查，仅负责生成符号表
class SemanticAnalyzer
{
public:
    SemanticAnalyzer(): root(nullptr) {}
    SemanticAnalyzer(AST *root): root(root) {}

    void semanticAnalysis();

    const SymbolTable &getSymbolTable() const {
        return symbolTable;
    }
private:
    void recognizeVariableDecl(const AST *const declNode, const string &scopeName, int &scopeIndex);
    void recognizeParam(const AST *const paramNode, const string &scopeName, int &scopeIndex);

    AST *root;
    SymbolTable symbolTable;
};