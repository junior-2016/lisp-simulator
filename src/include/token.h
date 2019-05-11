//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_TOKEN_H
#define LISP_SIMULATOR_TOKEN_H

#include "lisp.h"
#include "util.h"

namespace lisp {
    enum class TokenType {
        NUMBER, // 数值,支持整数模式或者浮点数模式

        LPAREN, // 左括号
        RPAREN, // 右括号

        DEFINE,  // "define"
        LAMBDA,  // "lambda"
        COND,    // "cond"
        // EQ 下放到 symbol_table 预定义符号里

        ATOM     // 除了数值,左右括号以及几个核心关键字以外的其他字符串都是atom
    };

    struct Token {
        TokenType type = TokenType::ATOM;
        std::variant<number_t, string_ptr> value;
    };

    TokenType getTokenByString(const string_t &string);
}
#endif //LISP_SIMULATOR_TOKEN_H
