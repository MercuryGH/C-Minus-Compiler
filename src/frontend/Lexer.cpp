#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <boost/format.hpp>
#include "Lexer.h"
#include "Token.h"
#include "TokenType.h"

using std::string;
using std::vector;

vector<Token> Lexer::lexicalAnalysis() const
{
    vector<Token> result;

    std::ifstream inputFile(readFilePath);
    string text;
    int curLineNo = 1;

    // read until '\0' (which means EOF)
    std::getline(inputFile, text, _EOF);

    const char *textPtr = text.c_str();
    Token curToken = getNextToken(textPtr, curLineNo);
    while (true)
    {
        result.push_back(curToken);

        if (curToken.getTokenType() == TokenType::END)
        {
            break;
        }
        curToken = getNextToken(textPtr, curLineNo);
    }

    return result;
}

void Lexer::throwInvalidCharErr(char curChar, int lineNo)
{
    throw std::runtime_error((
        boost::format("Invalid char %c found in line: %d") % curChar % lineNo
    ).str());
}

inline void Lexer::pushAndForward(const char *&textPtr, string &tokenStr) 
{
    tokenStr.push_back(*textPtr);
    textPtr++;
}

void Lexer::forwardStart(const char *&textPtr, LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo)
{
    if (isalpha(*textPtr) || *textPtr == '_')
    {
        lexingState = LexingState::IN_ID;
        pushAndForward(textPtr, tokenStr);
    }
    else if (isdigit(*textPtr))
    {
        lexingState = LexingState::IN_LITERAL;
        pushAndForward(textPtr, tokenStr);
    }
    else if (*textPtr == '\n') 
    {
        lineNo++;
        textPtr++;
    }
    else if (isspace(*textPtr))
    {
        textPtr++;
    }
    else
    {
        switch (*textPtr)
        {
            case '+':
                lexingState = LexingState::DONE;
                tokenType = TokenType::PLUS;
                pushAndForward(textPtr, tokenStr);
                break;

            case '-':
                lexingState = LexingState::DONE;
                tokenType = TokenType::MINUS;
                pushAndForward(textPtr, tokenStr);
                break;

            case '*':
                lexingState = LexingState::DONE;
                tokenType = TokenType::MULTIPLY;
                pushAndForward(textPtr, tokenStr);
                break;

            case '/':
                lexingState = LexingState::IN_SLASH;
                textPtr++;
                break;

            case '<':
                lexingState = LexingState::IN_LESS;
                pushAndForward(textPtr, tokenStr);
                break;

            case '>':
                lexingState = LexingState::IN_GREATER;
                pushAndForward(textPtr, tokenStr);
                break;

            case '=':
                lexingState = LexingState::IN_EQUAL;
                pushAndForward(textPtr, tokenStr);
                break;

            case '!':
                lexingState = LexingState::IN_NOT;
                pushAndForward(textPtr, tokenStr);
                break;

            case ';':
                lexingState = LexingState::DONE;
                tokenType = TokenType::SEMICOLON;
                pushAndForward(textPtr, tokenStr);
                break;

            case ',':
                lexingState = LexingState::DONE;
                tokenType = TokenType::COMMA;
                pushAndForward(textPtr, tokenStr);
                break;

            case '(':
                lexingState = LexingState::DONE;
                tokenType = TokenType::LEFT_ROUND_BRACKET;
                pushAndForward(textPtr, tokenStr);
                break;

            case ')':
                lexingState = LexingState::DONE;
                tokenType = TokenType::RIGHT_ROUND_BRACKET;
                pushAndForward(textPtr, tokenStr);
                break;

            case '[':
                lexingState = LexingState::DONE;
                tokenType = TokenType::LEFT_SQUARE_BRACKET;
                pushAndForward(textPtr, tokenStr);
                break;

            case ']':
                lexingState = LexingState::DONE;
                tokenType = TokenType::RIGHT_SQUARE_BRACKET;
                pushAndForward(textPtr, tokenStr);
                break;

            case '{':
                lexingState = LexingState::DONE;
                tokenType = TokenType::LEFT_CURLY_BRACKET;
                pushAndForward(textPtr, tokenStr);
                break;

            case '}':
                lexingState = LexingState::DONE;
                tokenType = TokenType::RIGHT_CURLY_BRACKET;
                pushAndForward(textPtr, tokenStr);
                break;

            case _EOF: 
                lexingState = LexingState::DONE;
                tokenType = TokenType::END;
                break;

            default:
                throwInvalidCharErr(*textPtr, lineNo);
        }
    }
}

void Lexer::forwardId(const char *&textPtr, LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &)
{
    // alpha or underscore or digit
    if (isalpha(*textPtr) || *textPtr == '_' || isdigit(*textPtr))
    {
        pushAndForward(textPtr, tokenStr);
    }
    else 
    {
        lexingState = LexingState::DONE;
        if (KEYWORD_MAP.find(tokenStr) != KEYWORD_MAP.end()) 
        {
            tokenType = KEYWORD_MAP.at(tokenStr);
        } 
        else 
        {
            tokenType = TokenType::IDENTIFIER;
        }
    }
}

void Lexer::forwardLiteral(const char *&textPtr, LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &) 
{
    if (isdigit(*textPtr))
    {
        pushAndForward(textPtr, tokenStr);
    }
    else 
    {
        lexingState = LexingState::DONE;
        tokenType = TokenType::LITERAL;
    }
}

void Lexer::forwardSlash(const char *&textPtr, LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &)
{
    if (*textPtr == '*')
    {
        lexingState = LexingState::IN_MULTILINE_COMMENT;
        textPtr++;
    } 
    else if (*textPtr == '/')
    {
        lexingState = LexingState::IN_SINGLELINE_COMMENT;
        textPtr++;
    } 
    else 
    {
        lexingState = LexingState::DONE;
        tokenType = TokenType::DIVIDE;
        tokenStr = "/";
    }
}

void Lexer::forwardSinglelineComment(const char *&textPtr, LexingState &lexingState, TokenType &, string &, int &lineNo)
{
    if (*textPtr == '\n') 
    {
        lineNo++;
        lexingState = LexingState::START;
    }
    textPtr++;
}

void Lexer::forwardMultilineComment(const char *&textPtr, LexingState &lexingState, TokenType &, string &, int &lineNo)
{
    if (*textPtr == '*')
    {
        lexingState = LexingState::END_MULTILINE_COMMENT;
    }
    else if (*textPtr == '\n')
    {
        lineNo++;
    }
    textPtr++;
}

void Lexer::forwardEndMultilineComment(const char *&textPtr, LexingState &lexingState, TokenType &, string &, int &lineNo)
{
    if (*textPtr == '/')
    {
        lexingState = LexingState::START;
    }
    else if (*textPtr != '*')
    {
        lexingState = LexingState::IN_MULTILINE_COMMENT;

        if (*textPtr == '\n')
        {
            lineNo++;
        }
    }

    textPtr++;
}

void Lexer::forwardLess(const char *&textPtr, LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &)
{
    if (*textPtr == '=')
    {
        tokenType = TokenType::LESS_EQUAL;
        pushAndForward(textPtr, tokenStr);
    }
    else
    {
        tokenType = TokenType::LESS;
    }

    lexingState = LexingState::DONE;
}

void Lexer::forwardGreater(const char *&textPtr, LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &)
{
    if (*textPtr == '=')
    {
        tokenType = TokenType::GREATER_EQUAL;
        pushAndForward(textPtr, tokenStr);
    }
    else
    {
        tokenType = TokenType::GREATER;
    }
    lexingState = LexingState::DONE;
}

void Lexer::forwardEqual(const char *&textPtr, LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &)
{
    if (*textPtr == '=')
    {
        tokenType = TokenType::EQUAL;
        pushAndForward(textPtr, tokenStr);
    }
    else
    {
        tokenType = TokenType::ASSIGN;
    }
    lexingState = LexingState::DONE;
}

void Lexer::forwardNot(const char *&textPtr, LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo)
{
    if (*textPtr == '=')
    {
        lexingState = LexingState::DONE;
        tokenType = TokenType::NOT_EQUAL;
        pushAndForward(textPtr, tokenStr);
    }
    else 
    {
        throwInvalidCharErr(*textPtr, lineNo);
    }
}

Token Lexer::getNextToken(const char *&textPtr, int &lineNo)
{
    LexingState lexingState = LexingState::START;
    TokenType tokenType;
    string tokenStr;

    while (true)
    {
        if (lexingState == LexingState::DONE) {
            break;
        }

        // std::cout << tokenStr << "\n";

        switch (lexingState)
        {
            case LexingState::START:
                forwardStart(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::IN_ID:
                forwardId(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::IN_LITERAL:
                forwardLiteral(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::IN_SLASH:
                forwardSlash(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::IN_SINGLELINE_COMMENT:
                forwardSinglelineComment(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::IN_MULTILINE_COMMENT:
                forwardMultilineComment(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::END_MULTILINE_COMMENT:
                forwardEndMultilineComment(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::IN_LESS:
                forwardLess(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::IN_GREATER:
                forwardGreater(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::IN_EQUAL:
                forwardEqual(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            case LexingState::IN_NOT:
                forwardNot(textPtr, lexingState, tokenType, tokenStr, lineNo);
                break;

            default:
                throw std::runtime_error("Invalid LexingState!");
        }
    }

    return Token(tokenType, tokenStr, lineNo);
}
