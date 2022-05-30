#pragma once

#include <unordered_map>
#include "Instruction.h"
#include "AST.h"
#include "Instruction.h"
#include "frontend/SemanticAnalyzer.h"

using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

// { funcName: [ Instrucions ] }
using InstructionMap = unordered_map<string, vector<Instruction>>;

class CodeGenerator
{
public:
    CodeGenerator(AST *root, const SymbolTable &symbolTable) : root(root),
                                                               symbolTable(symbolTable) {}

    vector<Instruction> generate() const;

private:
    AST *root;
    SymbolTable symbolTable;

    // inline const static string MAIN_NAME = "main";

    static void throwIdNotFoundErr(const string &id);

    vector<Instruction> generateLiteral(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateCompoundStmt(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateStmtList(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateStmt(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateIfStmt(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateWhileStmt(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateReturnStmt(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateExpr(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateLoadVariable(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateSimpleExpr(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateRelationOp(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateAddExpr(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateAddOp(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateTerm(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateMulOp(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateFactor(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateCall(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateNativeCall(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateArgList(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateAssign(
        const AST *root, const string &scopeName) const;

    vector<Instruction> generateGlobalVariables() const;

    vector<Instruction> generateCallMain() const;

    vector<Instruction> generateBeforeMain() const;

    InstructionMap createInstMap() const;

    static vector<Instruction> mergeInstMap(const InstructionMap &instMap, unordered_map<string, int> &funcCallMap);

    static void translateCall(vector<Instruction> &insts,
                              const unordered_map<string, int> &funcCallMap);
};
