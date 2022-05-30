#include <iostream>
#include "SemanticAnalyzer.h"
#include "VariableAttribute.h"
#include "Compiler.h"

void SemanticAnalyzer::semanticAnalysis()
{
    // function name cannot be $global
    symbolTable[GLOBAL_SCOPE_NAME] = {};

    int globalIndex = 0; // 索引全局变量（所有的全局变量视为一个数组）

    for (const AST *const declNode : root->getChildren())
    {
        // global variable
        if (declNode->getTokenType() == TokenType::VARIABLE_DECL)
        {
            recognizeVariableDecl(declNode, GLOBAL_SCOPE_NAME, globalIndex);
        }
        // global function
        else if (declNode->getTokenType() == TokenType::FUNC_DECL)
        {
            // FuncDecl -> Type id '(' Params ')' CompoundStmt
            // Params -> [ ParamList ]
            int localIndex = 0;

            const std::vector<AST *> &children = declNode->getChildren();
            const string &functionType = children.at(0)->getTokenStr();
            const string &functionName = children.at(1)->getTokenStr();

            symbolTable[functionName] = {};
            // params
            if (children.at(2) != nullptr)
            {
                for (const AST *const param : children.at(2)->getChildren())
                {
                    recognizeParam(param, functionName, localIndex);
                }
            }

            // local variables
            // CompoundStmt -> '{' LocalVariableDecl StmtList '}'
            // LocalVariableDecl -> { VariableDecl }
            for (const AST *const varDecl : children.at(3)->getChildren().at(0)->getChildren()) 
            {
                recognizeVariableDecl(varDecl, functionName, localIndex);
            }
        }
    }
}

// VariableDecl -> Type Id [ '[' literal ']' ]
/**
 * @details The number of symbol table entry 
 * for an array of size k is k + 1. e.g.
 * int a[3]; &a, a[0], a[1], a[2] are all 
 * in the symbol table, occupies 4 entries.
 * &a is the index of a.at(0).
 */
inline void SemanticAnalyzer::recognizeVariableDecl(const AST *const declNode, const string &scopeName, int &scopeIndex)
{
    // std::cout << "Test\n";
    const std::vector<AST *> &children = declNode->getChildren();
    const string &variableType = children.at(0)->getTokenStr();
    const string &variableName = children.at(1)->getTokenStr();

    // std::cout << "Set\n";

    VariableAttribute variableAttribute(scopeIndex, variableType);
    int varSize = 0;
    if (children.size() >= 3)
    {
        variableAttribute.varSize = std::stoi(children.at(2)->getTokenStr());
    }
    // symbolTable[scopeName][variableName] = variableAttribute;
    symbolTable[scopeName].emplace(variableName, variableAttribute);
    scopeIndex += varSize + 1;
}

// Param -> Type id [ '[' ']' ]
inline void SemanticAnalyzer::recognizeParam(const AST *const paramNode, const string &scopeName, int &scopeIndex)
{
    // std::cout << "TestParam\n";

    // doesn't recognize array since passing by pointer
    const std::vector<AST *> &paramChildren = paramNode->getChildren();
    const string &variableType = paramChildren.at(0)->getTokenStr();
    const string &variableName = paramChildren.at(1)->getTokenStr();

    // std::cout << "SetParam\n";

    VariableAttribute variableAttribute(scopeIndex, variableType);

    symbolTable[scopeName].emplace(variableName, variableAttribute);
    scopeIndex++;
}
