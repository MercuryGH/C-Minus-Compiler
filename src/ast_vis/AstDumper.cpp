#include <iostream>
#include <string>
#include "ast_vis/AstDumper.h"

using std::string;

/**
 * @brief Firstorder traverse AST
 * 
 * @param root of AST
 * @return Json::Value Serialized JSON representation of the AST
 */
Json::Value AstDumper::dump(const AST *root)
{
    Json::Value ret;

    switch (root->getTokenType()) {
        case TokenType::INT:
        case TokenType::VOID:
        case TokenType::CHAR:
        case TokenType::FLOAT:
        case TokenType::DOUBLE:
            ret["type"] = root->getTokenStr();
            break;
        case TokenType::IDENTIFIER:
            ret["id"] = root->getTokenStr();
            break;
        case TokenType::LITERAL:
            ret["literal"] = root->getTokenStr();
            break;
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::ASSIGN:
            ret["operator"] = root->getTokenStr();
            break;
        default:
            ret["node_type"] = root->getTokenStr();
    }
    
    for (const auto child : root->getChildren()) {
        if (child != nullptr) {
            ret["children"].append(dump(child));
        }
    }

    return ret;
}