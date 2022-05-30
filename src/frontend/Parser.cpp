#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <boost/format.hpp>
#include "Parser.h"

using std::string;
using std::vector;

AST *Parser::syntaxAnalysis()
{
    AST *root = nullptr;
    // if (tokenList.empty()) {
    //     return nullptr;
    // }
    auto tokenPtr = tokenList.data();

    Program(root, tokenPtr);

    return root;
}

void Parser::throwInvalidTokenErr(const Token *tokenPtr)
{
    const string tokenStr = tokenPtr->getTokenStr();
    const int lineNo = tokenPtr->getLineNo();
    throw std::runtime_error((
        boost::format("Invalid token %s found in line: %d") % tokenStr % lineNo)
    .str());
}

void Parser::matchToken(TokenType tokenType, Token *&tokenPtr)
{
    if (tokenPtr->getTokenType() == tokenType)
    {
        tokenPtr++;
    }
    else
    {
        throwInvalidTokenErr(tokenPtr);
    }
}

/**
 * @EBNF Program -> DeclList
 */
void Parser::Program(AST *&root, Token *&tokenPtr)
{
    DeclList(root, tokenPtr);
}

/**
 * @EBNF DeclList -> Decl { Decl }
 */
void Parser::DeclList(AST *&root, Token *&tokenPtr)
{
    std::vector<AST *> children = {nullptr};
    root = new AST(TokenType::DECL_LIST, "DeclList", children);

    // std::cout << "Before Decl\n";

    Decl(root->getChildren().at(0), tokenPtr);

    // std::cout << "Before while()\n";

    while (true) {
        if (tokenPtr->getTokenType() == TokenType::END) {
            break;
        }
        root->getChildren().push_back(nullptr);
        
        // std::cout << "BBefore Decl\n";

        Decl(root->getChildren().back(), tokenPtr);
    }
}

/**
 * @EBNF Decl -> VarDecl | FunDecl
 */
void Parser::Decl(AST *&root, Token *&tokenPtr)
{
    // check for first set
    if (tokenPtr->isTypeToken() == false) {
        throwInvalidTokenErr(tokenPtr);
    }
    // std::cout << "Decl 1\n";

    // look one more token
    TokenType secondTokenType = tokenPtr[1].getTokenType();
    if (secondTokenType != TokenType::IDENTIFIER) {
        throwInvalidTokenErr(tokenPtr + 1);
    }

    // std::cout << "Decl 2\n";

    // look two more token
    TokenType thirdTokenType = tokenPtr[2].getTokenType();
    if (thirdTokenType == TokenType::LEFT_SQUARE_BRACKET || thirdTokenType == TokenType::SEMICOLON) {
        VariableDecl(root, tokenPtr);
    } else if (thirdTokenType == TokenType::LEFT_ROUND_BRACKET) {
        FuncDecl(root, tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr + 2);
    }
}

/**
 * @EBNF VariableDecl -> Type Id [ '[' literal ']' ] ';'
 */
void Parser::VariableDecl(AST *&root, Token *&tokenPtr)
{
    std::vector<AST *> children = {nullptr, nullptr};
    root = new AST(TokenType::VARIABLE_DECL, "VariableDecl", children);

    Type(root->getChildren().at(0), tokenPtr);

    if (tokenPtr->getTokenType() == TokenType::IDENTIFIER) {
        root->getChildren().at(1) = new AST(tokenPtr);
        matchToken(TokenType::IDENTIFIER, tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }
    if (tokenPtr->getTokenType() == TokenType::LEFT_SQUARE_BRACKET) {
        matchToken(TokenType::LEFT_SQUARE_BRACKET, tokenPtr);

        root->getChildren().push_back(new AST(tokenPtr));

        matchToken(TokenType::LITERAL, tokenPtr);
        matchToken(TokenType::RIGHT_SQUARE_BRACKET, tokenPtr);
    }
    matchToken(TokenType::SEMICOLON, tokenPtr);
}

/**
 * @EBNF Type -> int | void
 */
void Parser::Type(AST *&root, Token *&tokenPtr) 
{
    if (tokenPtr->isTypeToken()) {
        root = new AST(tokenPtr);
        matchToken(tokenPtr->getTokenType(), tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }
}

/**
 * @EBNF FuncDecl -> Type id '(' Params ')' CompoundStmt
 */
void Parser::FuncDecl(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr, nullptr, nullptr, nullptr};
    root = new AST(TokenType::FUNC_DECL, "FuncDecl", children);

    // std::cout << "FuncDecl\n";
    
    Type(root->getChildren().at(0), tokenPtr);

    // std::cout << "After Type\n";

    if (tokenPtr->getTokenType() == TokenType::IDENTIFIER) {
        root->getChildren().at(1) = new AST(tokenPtr);
        
        matchToken(TokenType::IDENTIFIER, tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }

    // std::cout << "C1\n";

    matchToken(TokenType::LEFT_ROUND_BRACKET, tokenPtr);
    Params(root->getChildren().at(2), tokenPtr);

    // std::cout << "Param\n";

    matchToken(TokenType::RIGHT_ROUND_BRACKET, tokenPtr);
    CompoundStmt(root->getChildren().at(3), tokenPtr);
}

/**
 * @EBNF Params -> [ParamList]
 */
void Parser::Params(AST *&root, Token *&tokenPtr) 
{
    if (tokenPtr->isTypeToken()) {
        ParamList(root, tokenPtr);
    }
}

/**
 * @EBNF ParamList -> Param { ',' Param }
 */
void Parser::ParamList(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr};
    root = new AST(TokenType::PARAM_LIST, "ParamList", children);

    Param(root->getChildren().at(0), tokenPtr);
    while (true) {
        if (tokenPtr->getTokenType() != TokenType::COMMA) {
            break;
        }
        matchToken(TokenType::COMMA, tokenPtr);

        root->getChildren().push_back(nullptr);
        Param(root->getChildren().back(), tokenPtr);
    }
}

/**
 * @EBNF Param -> Type id [ '[' ']' ]
 */
void Parser::Param(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr, nullptr};
    root = new AST(TokenType::PARAM, "Param", children);
    Type(root->getChildren().at(0), tokenPtr);

    if (tokenPtr->getTokenType() == TokenType::IDENTIFIER) {
        root->getChildren().at(1) = new AST(tokenPtr);
        matchToken(TokenType::IDENTIFIER, tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }
    
    // has [] 
    if (tokenPtr->getTokenType() == TokenType::LEFT_SQUARE_BRACKET) {
        matchToken(TokenType::LEFT_SQUARE_BRACKET, tokenPtr);
        matchToken(TokenType::RIGHT_SQUARE_BRACKET, tokenPtr);
    }
}

/**
 * @EBNF CompoundStmt -> '{' LocalVariableDecl StmtList '}'
 * 
 * @details Note that local variable declaration is only 
 * allowed in the beginning of a CompoundStmt. In early JS, 
 * varaible hoisting is implemented because of the similar reason.
 */
void Parser::CompoundStmt(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr, nullptr};
    root = new AST(TokenType::COMPOUND_STMT, "CompoundStmt", children);

    matchToken(TokenType::LEFT_CURLY_BRACKET, tokenPtr);

    LocalVariableDecl(root->getChildren().at(0), tokenPtr);

    // std::cout << "CS\n";

    StmtList(root->getChildren().at(1), tokenPtr);

    matchToken(TokenType::RIGHT_CURLY_BRACKET, tokenPtr);
}

/**
 * @EBNF LocalVariableDecl -> { VariableDecl }
 */
void Parser::LocalVariableDecl(AST *&root, Token *&tokenPtr) 
{
    root = new AST(TokenType::LOCAL_VARIABLE_DECL, "LocalVariableDecl");

    while (tokenPtr->isTypeToken()) {
        root->getChildren().push_back(nullptr);
        VariableDecl(root->getChildren().back(), tokenPtr);
    }
}

/**
 * @EBNF StmtList -> { Stmt }
 */
void Parser::StmtList(AST *&root, Token *&tokenPtr) 
{
    root = new AST(TokenType::STMT_LIST, "StmtList");

    while (true) {
        // first set
        if ((tokenPtr->getTokenType() == TokenType::SEMICOLON ||
            tokenPtr->getTokenType() == TokenType::IDENTIFIER ||
            tokenPtr->getTokenType() == TokenType::LEFT_ROUND_BRACKET ||
            tokenPtr->getTokenType() == TokenType::LITERAL ||
            tokenPtr->getTokenType() == TokenType::LEFT_CURLY_BRACKET ||
            tokenPtr->getTokenType() == TokenType::IF ||
            tokenPtr->getTokenType() == TokenType::WHILE ||
            tokenPtr->getTokenType() == TokenType::RETURN) == false
        ) {
            break;
        }

        // std::cout << "StmtList\n";

        root->getChildren().push_back(nullptr);
        Stmt(root->getChildren().back(), tokenPtr);
    }
}

/**
 * @EBNF Stmt -> ExprStmt | CompoundStmt | IfStmt | WhileStmt | ReturnStmt
 */
void Parser::Stmt(AST *&root, Token *&tokenPtr) 
{
    // std::cout << "TestStmt\n";
    // first set
    if (tokenPtr->getTokenType() == TokenType::SEMICOLON ||
        tokenPtr->getTokenType() == TokenType::IDENTIFIER ||
        tokenPtr->getTokenType() == TokenType::LEFT_ROUND_BRACKET ||
        tokenPtr->getTokenType() == TokenType::LITERAL
    ) {
        ExprStmt(root, tokenPtr);
    } else if (tokenPtr->getTokenType() == TokenType::LEFT_CURLY_BRACKET) {
        CompoundStmt(root, tokenPtr);
    } else if (tokenPtr->getTokenType() == TokenType::IF) {
        IfStmt(root, tokenPtr);
    } else if (tokenPtr->getTokenType() == TokenType::WHILE) {
        WhileStmt(root, tokenPtr);
    } else if (tokenPtr->getTokenType() == TokenType::RETURN) {
        ReturnStmt(root, tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }
}

/**
 * @EBNF ExprStmt -> [ Expr ] ';'
 */
void Parser::ExprStmt(AST *&root, Token *&tokenPtr) 
{
    // match expr (first set)
    if (tokenPtr->getTokenType() == TokenType::IDENTIFIER ||
        tokenPtr->getTokenType() == TokenType::LEFT_ROUND_BRACKET ||
        tokenPtr->getTokenType() == TokenType::LITERAL
    ) {
        Expr(root, tokenPtr);
    }

    matchToken(TokenType::SEMICOLON, tokenPtr);
}

/**
 * @EBNF IfStmt -> if '(' Expr ')' Stmt [ else Stmt ]
 * 
 * @details dangling else.
 */
void Parser::IfStmt(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr, nullptr};
    root = new AST(TokenType::IF_STMT, "IfStmt", children);

    matchToken(TokenType::IF, tokenPtr);
    matchToken(TokenType::LEFT_ROUND_BRACKET, tokenPtr);

    // std::cout << "if stmt\n";

    Expr(root->getChildren().at(0), tokenPtr);

    // std::cout << "after expr\n";

    matchToken(TokenType::RIGHT_ROUND_BRACKET, tokenPtr);

    Stmt(root->getChildren().at(1), tokenPtr);

    // std::cout << "after Stmt\n";

    if (tokenPtr->getTokenType() == TokenType::ELSE) {
        matchToken(TokenType::ELSE, tokenPtr);

        root->getChildren().push_back(nullptr);
        Stmt(root->getChildren().at(2), tokenPtr);
        // std::cout << "after Stmt2\n";
    }
}

/**
 * @EBNF WhileStmt -> while '(' Expr ')' Stmt
 */
void Parser::WhileStmt(AST *&root, Token *&tokenPtr) 
{
    // std::cout << "While DEBUG\n";

    std::vector<AST *> children = {nullptr, nullptr};
    root = new AST(TokenType::WHILE_STMT, "WhileStmt", children);

    matchToken(TokenType::WHILE, tokenPtr);
    matchToken(TokenType::LEFT_ROUND_BRACKET, tokenPtr);

    Expr(root->getChildren().at(0), tokenPtr);

    matchToken(TokenType::RIGHT_ROUND_BRACKET, tokenPtr);

    Stmt(root->getChildren().at(1), tokenPtr);
}

/**
 * @EBNF ReturnStmt -> return [ Expr ] ';'
 */
void Parser::ReturnStmt(AST *&root, Token *&tokenPtr) 
{
    root = new AST(TokenType::RETURN_STMT, "ReturnStmt");
    matchToken(TokenType::RETURN, tokenPtr);

    // match expr (first set)
    if (tokenPtr->getTokenType() == TokenType::IDENTIFIER ||
        tokenPtr->getTokenType() == TokenType::LEFT_ROUND_BRACKET ||
        tokenPtr->getTokenType() == TokenType::LITERAL
    ) {
        root->getChildren().push_back(nullptr);
        Expr(root->getChildren().back(), tokenPtr);
    }

    matchToken(TokenType::SEMICOLON, tokenPtr);
}

/**
 * @EBNF Expr -> SimpleExpr | Variable '=' Expr
 * 
 * @details Expr is SimpleExpr or AssignmentExpr
 * conflict: First(SimpleExpr) ∩ First(Variable) == { id }
 * which is resolved by looking Variable.
 */
void Parser::Expr(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr};
    root = new AST(TokenType::EXPR, "Expr", children);

    // start with parenthesis or literal
    // must be SimpleExpr / SimpleExpr
    if (tokenPtr->getTokenType() == TokenType::LEFT_ROUND_BRACKET ||
        tokenPtr->getTokenType() == TokenType::LITERAL
    ) {
        SimpleExpr(root->getChildren().at(0), tokenPtr);
        return;
    }

    if (tokenPtr->getTokenType() == TokenType::IDENTIFIER) {
        // look one more
        // Function call, derive SimpleExpr
        if (tokenPtr[1].getTokenType() == TokenType::LEFT_ROUND_BRACKET) {
            SimpleExpr(root->getChildren().at(0), tokenPtr);
        } else {
            Token *tmpTokenPtr = tokenPtr;

            Variable(root->getChildren().at(0), tokenPtr);

            // std::cout << "After Variable\n";

            bool isAssignOp = tokenPtr->getTokenType() == TokenType::ASSIGN;
            AST::destroyChildrenRecursively(root);
            // backtrack
            tokenPtr = tmpTokenPtr;

            if (isAssignOp) {
                root->getChildren().push_back(nullptr);
                Variable(root->getChildren().at(0), tokenPtr);
                matchToken(TokenType::ASSIGN, tokenPtr);

                Expr(root->getChildren().at(1), tokenPtr);
            } else {
                // std::cout << "SimpleExpr\n";

                SimpleExpr(root->getChildren().at(0), tokenPtr);
            }
        }
    } else {
        throwInvalidTokenErr(tokenPtr);
    }
}

/**
 * @EBNF Variable -> id [ '[' Expr ']' ]
 * 
 * @details support dereference for array variable.
 */
void Parser::Variable(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr};
    root = new AST(TokenType::VARIABLE, "Variable", children); 

    if (tokenPtr->getTokenType() == TokenType::IDENTIFIER) {
        root->getChildren().at(0) = new AST(tokenPtr);
        matchToken(TokenType::IDENTIFIER, tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }

    // array index
    if (tokenPtr->getTokenType() == TokenType::LEFT_SQUARE_BRACKET) {
        matchToken(TokenType::LEFT_SQUARE_BRACKET, tokenPtr);

        root->getChildren().push_back(nullptr);
        Expr(root->getChildren().at(1), tokenPtr);

        matchToken(TokenType::RIGHT_SQUARE_BRACKET, tokenPtr);
    }
}

/**
 * @EBNF SimpleExpr -> AddExpr [ RelationOp AddExpr ]
 * 
 * @details RelationOp is nullable
 */
void Parser::SimpleExpr(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr};
    root = new AST(TokenType::SIMPLE_EXPR, "SimpleExpr", children);

    AddExpr(root->getChildren().at(0), tokenPtr);

    if (tokenPtr->isRelationOpToken()) {
        root->getChildren().push_back(nullptr);
        RelationOp(root->getChildren().at(1), tokenPtr);

        root->getChildren().push_back(nullptr);
        AddExpr(root->getChildren().at(2), tokenPtr);
    }
}

/**
 * @EBNF RelationOp -> < | <= | > | >= | == | !=
 */
void Parser::RelationOp(AST *&root, Token *&tokenPtr) 
{
    if (tokenPtr->isRelationOpToken()) {
        root = new AST(tokenPtr);
        matchToken(tokenPtr->getTokenType(), tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }
}

/**
 * @EBNF AddExpr -> Term { AddOp Term }
 */
void Parser::AddExpr(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr};
    root = new AST(TokenType::ADD_EXPR, "AddExpr", children);

    Term(root->getChildren().at(0), tokenPtr);

    while (true) {
        if (tokenPtr->getTokenType() != TokenType::PLUS && tokenPtr->getTokenType() != TokenType::MINUS) {
            break;
        }

        root->getChildren().push_back(nullptr);
        AddOp(root->getChildren().back(), tokenPtr);

        root->getChildren().push_back(nullptr);
        Term(root->getChildren().back(), tokenPtr);
    }
}

/**
 * @EBNF AddOp -> + | -
 */
void Parser::AddOp(AST *&root, Token *&tokenPtr) 
{
    if (tokenPtr->getTokenType() == TokenType::PLUS || tokenPtr->getTokenType() == TokenType::MINUS) {
        root = new AST(tokenPtr);
        matchToken(tokenPtr->getTokenType(), tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }
}

/**
 * @EBNF Term -> Factor { MulOp Factor }
 */
void Parser::Term(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr};
    root = new AST(TokenType::TERM, "Term", children);

    Factor(root->getChildren().at(0), tokenPtr);

    while (true) {
        if (tokenPtr->getTokenType() != TokenType::MULTIPLY && tokenPtr->getTokenType() != TokenType::DIVIDE) {
            break;
        }

        root->getChildren().push_back(nullptr);
        MulOp(root->getChildren().back(), tokenPtr);

        root->getChildren().push_back(nullptr);
        Factor(root->getChildren().back(), tokenPtr);
    }
}

/**
 * @EBNF MulOp -> * | /
 */
void Parser::MulOp(AST *&root, Token *&tokenPtr) 
{
    if (tokenPtr->getTokenType() == TokenType::MULTIPLY || tokenPtr->getTokenType() == TokenType::DIVIDE) {
        root = new AST(tokenPtr);
        matchToken(tokenPtr->getTokenType(), tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }
}

/**
 * @EBNF Factor -> '(' Expr ')' | Variable | Call | literal
 */
void Parser::Factor(AST *&root, Token *&tokenPtr) 
{
    if (tokenPtr->getTokenType() == TokenType::LEFT_ROUND_BRACKET) {
        matchToken(TokenType::LEFT_ROUND_BRACKET, tokenPtr);

        Expr(root, tokenPtr);

        matchToken(TokenType::RIGHT_ROUND_BRACKET, tokenPtr);
    } else if (tokenPtr->getTokenType() == TokenType::LITERAL) {
        root = new AST(tokenPtr);

        matchToken(tokenPtr->getTokenType(), tokenPtr);
    } else if (tokenPtr->getTokenType() == TokenType::IDENTIFIER) {
        if (tokenPtr[1].getTokenType() == TokenType::LEFT_ROUND_BRACKET) {
            Call(root, tokenPtr);
        } else {
            Variable(root, tokenPtr);
        }
    } else {
        throwInvalidTokenErr(tokenPtr);
    }
}

/**
 * @EBNF Call -> id '(' [ ArgList ] ')'
 */
void Parser::Call(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr};
    root = new AST(TokenType::CALL, "Call", children);

    if (tokenPtr->getTokenType() == TokenType::IDENTIFIER) {
        root->getChildren().at(0) = new AST(tokenPtr);
        matchToken(TokenType::IDENTIFIER, tokenPtr);
    } else {
        throwInvalidTokenErr(tokenPtr);
    }

    matchToken(TokenType::LEFT_ROUND_BRACKET, tokenPtr);

    if (tokenPtr->getTokenType() == TokenType::IDENTIFIER         ||
        tokenPtr->getTokenType() == TokenType::LEFT_ROUND_BRACKET ||
        tokenPtr->getTokenType() == TokenType::LITERAL
    ) {
        root->getChildren().push_back(nullptr);
        ArgList(root->getChildren().at(1), tokenPtr);
    }

    matchToken(TokenType::RIGHT_ROUND_BRACKET, tokenPtr);
}

/**
 * @EBNF ArgList -> Expr { ',' Expr }
 */
void Parser::ArgList(AST *&root, Token *&tokenPtr) 
{
    std::vector<AST *> children = {nullptr};
    root = new AST(TokenType::ARG_LIST, "ArgList", children);

    Expr(root->getChildren().at(0), tokenPtr);

    while (true) {
        if (tokenPtr->getTokenType() != TokenType::COMMA) {
            break;
        }
        matchToken(TokenType::COMMA, tokenPtr);

        root->getChildren().push_back(nullptr);
        Expr(root->getChildren().back(), tokenPtr);
    }
}