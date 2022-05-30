#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "LexingState.h"
#include "Token.h"
#include "TokenType.h"

#define _EOF '\0'

using std::string;
using std::vector;

class Lexer
{
public:
    Lexer(): readFilePath("") {}
    Lexer(const string &readFilePath): readFilePath(readFilePath) {}

    vector<Token> lexicalAnalysis() const;

private:
    string readFilePath;

    // 非法字符，报错
    static void throwInvalidCharErr(char curChar, int lineNo);

    // token += char, charPtr++;
    static void pushAndForward(const char *&textPtr, string &tokenStr);

    // 开始
    static void forwardStart(const char *&textPtr,
                             LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // 期望读下一个字
    static void forwardId(const char *&textPtr,
                          LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // 期望读下一个数
    static void forwardLiteral(const char *&textPtr,
                              LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // 除号 或者 注释
    static void forwardSlash(const char *&textPtr,
                             LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // 单行注释，读直到下一行开始
    static void forwardSinglelineComment(const char *&textPtr,
                                         LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // 还是多行注释 或者 多行注释要结束 (*)
    static void forwardMultilineComment(const char *&textPtr,
                                        LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // 还是多行注释 或者 多行注释要结束 (/)
    static void forwardEndMultilineComment(const char *&textPtr,
                                           LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // < 或者 <=
    static void forwardLess(const char *&textPtr,
                            LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // > 或者 >=
    static void forwardGreater(const char *&textPtr,
                               LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // = 或者 ==
    static void forwardEqual(const char *&textPtr,
                              LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // != 或者非法字符
    static void forwardNot(const char *&textPtr,
                           LexingState &lexingState, TokenType &tokenType, string &tokenStr, int &lineNo);

    // 获取下一个 token，循环调用
    static Token getNextToken(const char *&textPtr, int &lineNo);
};