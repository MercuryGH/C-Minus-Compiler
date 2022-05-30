#pragma once

#include <string>
#include "TokenType.h"

using std::string;
using std::ostream;

class Token
{
public:
    Token(TokenType tokenType, const string &tokenStr, int lineNo):
        tokenType(tokenType), tokenStr(tokenStr), lineNo(lineNo) {}

    TokenType getTokenType() const {
        return tokenType;
    }
    string &getTokenStr() {
        return tokenStr;
    }
    const string &getTokenStr() const {
        return tokenStr;
    }
    int getLineNo() const {
        return lineNo;
    }

    bool isTypeToken() const {
        return tokenType == TokenType::INT || tokenType == TokenType::VOID;
    }

    bool isKeywordToken() const {
        return KEYWORD_MAP.find(tokenStr) != KEYWORD_MAP.end();
    }

    bool isRelationOpToken() const {
        return tokenType == TokenType::LESS       ||
            tokenType == TokenType::LESS_EQUAL    ||
            tokenType == TokenType::GREATER       ||
            tokenType == TokenType::GREATER_EQUAL ||
            tokenType == TokenType::EQUAL         ||
            tokenType == TokenType::NOT_EQUAL;
    }

    friend ostream &operator<<(ostream &out, Token t) {
        out << t.tokenStr;
        return out;
    }

private:
    TokenType tokenType;
    string tokenStr;
    int lineNo;
};
