#pragma once

#include "AST.h"
#include "Token.h"

/**
 * @brief recursive descent LL(*) parser constructed manually by EBNF
 * @ref https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
 * @ref http://pfmiles.github.io/blog/concept-and-implementation-of-ll-star/ 
 */
class Parser
{
public:
    Parser() : tokenList(vector<Token>()) {}
    Parser(const vector<Token> &tokenList) : tokenList(tokenList) {}

    // return root of AST
    AST *syntaxAnalysis();

private:
    vector<Token> tokenList;

    static void throwInvalidTokenErr(const Token *tokenPtr);

    static void matchToken(TokenType tokenType, Token *&tokenPtr);

    // EBNF non-terminals

    static void Program(AST *&root, Token *&tokenPtr);

    static void DeclList(AST *&root, Token *&tokenPtr);

    static void Decl(AST *&root, Token *&tokenPtr);

    static void VariableDecl(AST *&root, Token *&tokenPtr);

    static void Type(AST *&root, Token *&tokenPtr);

    static void FuncDecl(AST *&root, Token *&tokenPtr);

    static void Params(AST *&root, Token *&tokenPtr);

    static void ParamList(AST *&root, Token *&tokenPtr);

    static void Param(AST *&root, Token *&tokenPtr);

    static void CompoundStmt(AST *&root, Token *&tokenPtr);

    static void LocalVariableDecl(AST *&root, Token *&tokenPtr);

    static void StmtList(AST *&root, Token *&tokenPtr);

    static void Stmt(AST *&root, Token *&tokenPtr);

    static void ExprStmt(AST *&root, Token *&tokenPtr);

    static void IfStmt(AST *&root, Token *&tokenPtr);

    static void WhileStmt(AST *&root, Token *&tokenPtr);

    static void ReturnStmt(AST *&root, Token *&tokenPtr);

    static void Expr(AST *&root, Token *&tokenPtr);

    static void Variable(AST *&root, Token *&tokenPtr);

    static void SimpleExpr(AST *&root, Token *&tokenPtr);

    static void RelationOp(AST *&root, Token *&tokenPtr);

    static void AddExpr(AST *&root, Token *&tokenPtr);

    static void AddOp(AST *&root, Token *&tokenPtr);

    static void Term(AST *&root, Token *&tokenPtr);

    static void MulOp(AST *&root, Token *&tokenPtr);

    static void Factor(AST *&root, Token *&tokenPtr);

    static void Call(AST *&root, Token *&tokenPtr);

    static void ArgList(AST *&root, Token *&tokenPtr);
};

/*

EBNF for C-Minus

Program -> DeclList
DeclList -> Decl { Decl }
Decl -> VariableDecl | FuncDecl
VariableDecl -> Type id [ '[' literal ']' ]
Type -> int | void
FuncDecl -> Type id '(' Params ')' CompoundStmt
Params -> [ParamList]
ParamList -> Param { ',' Param }
Param -> Type id [ '[' ']' ]
CompoundStmt -> '{' LocalVariableDecl StmtList '}'
LocalVariableDecl -> { VariableDecl }
StmtList -> { Stmt }
Stmt -> ExprStmt | CompoundStmt | IfStmt | WhileStmt | ReturnStmt
ExprStmt -> [ Expr ] ';'
IfStmt -> if '(' Expr ')' Stmt [ else Stmt ]
WhileStmt -> while '(' Expr ')' Stmt
ReturnStmt -> return [ Expr ] ';'
Expr -> SimpleExpr | Variable '=' Expr
Variable -> id [ '[' Expr ']' ]
SimpleExpr -> AddExpr [ RelationOp AddExpr ]
RelationOp -> < | <= | > | >= | == | !=
AddExpr -> Term { AddOp Term }
AddOp -> + | -
Term -> Factor { MulOp Factor }
MulOp -> * | /
Factor -> '(' Expr ')' | Variable | Call | literal
Call -> id '(' [ ArgList ] ')'
ArgList -> Expr { ',' Expr }

Example of conflict (Non-LL(1)):
First(SimpleExpr) = { '(', id, literal }
First(Variable) = { id }

Optimization:
Second(SimpleExpr) contains '(' from Call
Second(Variable) doesn't contain '('

But we still need to see whether '=' can be derived.
*/
