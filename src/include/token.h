//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_TOKEN_H
#define LISP_SIMULATOR_TOKEN_H

#include "lisp.h"

namespace lisp {
    enum class TokenType {
        NUMBER, // 数值,支持整数模式或者浮点数模式

        LPAREN, // 左括号
        RPAREN, // 右括号

        DEFINE,  // "define"
        LAMBDA,  // "lambda"

        // EQ COND 下放到 symbol_table 预定义符号里
        // EQ,      // "eq?"
        // COND,    // "cond"

        ATOM     // 除了数值,左右括号以及几个核心关键字以外的其他字符串都是atom
    };

    using Token = struct {
        TokenType type = TokenType::ATOM;
        std::variant<number_t, string_ptr> value;
    };

    TokenType getTokenByString(const string_t &string) {
        if (string == "define") {
            return TokenType::DEFINE;
        } else if (string == "lambda") {
            return TokenType::LAMBDA;
        } else if (string == "(") {
            return TokenType::LPAREN;
        } else if (string == ")") {
            return TokenType::RPAREN;
        } else if (is_number(string)) {
            return TokenType::NUMBER;
        } else {
            return TokenType::ATOM;
        }
    }
}
#endif //LISP_SIMULATOR_TOKEN_H
