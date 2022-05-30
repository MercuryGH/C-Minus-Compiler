#pragma once

#include <unordered_set>
#include <unordered_map>

// TODO: 没有浮点数，懒得做
// TODO: 不支持与或非

/**
 * AstLeafType is TokenType
 * AstNonLeafType is AstType
 */
enum class TokenType
{
    // Literal
    IDENTIFIER,
    LITERAL,

    // Keyword
    VOID,
    INT,
    CHAR,
    FLOAT,
    DOUBLE,
    IF,
    ELSE,
    WHILE,
    RETURN,

    // Operator
    PLUS,                 // +
    MINUS,                // -
    MULTIPLY,             // *
    DIVIDE,               // /
    LESS,                 // <
    LESS_EQUAL,           // <=
    GREATER,              // >
    GREATER_EQUAL,        // >=
    EQUAL,                // ==
    NOT_EQUAL,            // !=
    ASSIGN,               // =
    SEMICOLON,            // ;
    COMMA,                // ,
    LEFT_ROUND_BRACKET,   // (
    RIGHT_ROUND_BRACKET,  // )
    LEFT_SQUARE_BRACKET,  // [
    RIGHT_SQUARE_BRACKET, // ]
    LEFT_CURLY_BRACKET,   // {
    RIGHT_CURLY_BRACKET,  // }

    // End of Lexing one token
    END,

    // AstType
    DECL_LIST,
    VARIABLE_DECL,
    FUNC_DECL,
    PARAM_LIST,
    PARAM,
    COMPOUND_STMT, // CompoundStmt表示一个 {} 包括的代码块
    LOCAL_VARIABLE_DECL,
    STMT_LIST,
    IF_STMT,
    WHILE_STMT,
    RETURN_STMT,
    EXPR,
    VARIABLE,
    SIMPLE_EXPR,
    ADD_EXPR,
    TERM,
    CALL,
    ARG_LIST,
};

const std::unordered_map<std::string, TokenType> KEYWORD_MAP{
    {"void", TokenType::VOID},
    {"int", TokenType::INT},
    {"char", TokenType::CHAR},
    {"float", TokenType::FLOAT},
    {"double", TokenType::DOUBLE},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN},
};

