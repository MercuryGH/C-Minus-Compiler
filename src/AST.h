#pragma once

#include <string>
#include <vector>
#include "frontend/TokenType.h"
#include "frontend/Token.h"

using std::string;
using std::vector;

class AST
{
public:
    AST(TokenType tokenType, const string &tokenStr, const vector<AST *> &children, int lineNo): 
        tokenType(tokenType),
        tokenStr(tokenStr), 
        children(children),
        lineNo(lineNo) {}

    AST(TokenType tokenType, const string &tokenStr, const vector<AST *> &children): 
        tokenType(tokenType),
        tokenStr(tokenStr), 
        children(children),
        lineNo(0) {}

    AST(TokenType tokenType, const string &tokenStr): 
        tokenType(tokenType),
        tokenStr(tokenStr), 
        children(vector<AST *>()),
        lineNo(0) {}

    AST(const Token *tokenPtr):
        tokenType(tokenPtr->getTokenType()),
        tokenStr(tokenPtr->getTokenStr()),
        children(vector<AST *>()),
        lineNo(tokenPtr->getLineNo()) {}

    TokenType getTokenType() const {
        return tokenType;
    }

    string &getTokenStr() {
        return tokenStr;
    }
    const string &getTokenStr() const {
        return tokenStr;
    }

    const vector<AST *> &getChildren() const {
        return children;
    }
    vector<AST *> &getChildren() {
        return children;
    }

    int getLineNo() const {
        return lineNo;
    }

    /**
     * @brief Postorder delete AST (root = node)
     * Only node will be remained.
     */
    static void destroyChildrenRecursively(AST *node) {
        if (node == nullptr) {
            return;
        }

        for (auto child : node->children) {
            destroyChildrenRecursively(child);
            delete child;
        }
    }

    ~AST() {
        // destroyChildrenRecursively(this); Double free
    }

private:
    TokenType tokenType;
    string tokenStr;
    vector<AST *> children;
    int lineNo;
};