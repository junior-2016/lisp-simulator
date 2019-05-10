//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_TOKEN_H
#define LISP_SIMULATOR_TOKEN_H

#include "lisp.h"

namespace lisp {
    enum class Token {
        NUMBER, // 数值,支持整数模式或者浮点数模式

        LPAREN, // 左括号
        RPAREN, // 右括号

        DEFINE,  // "define"
        LAMBDA,  // "lambda"
        EQ,      // "eq?"
        COND,    // "cond"

        ATOM     // 除了数值,左右括号以及几个核心关键字以外的其他字符串都是atom
    };

    using TokenRet = struct {
        Token token = Token::ATOM;
        std::variant<number_t, string_ptr> value;
    };

    Token getTokenByString(const string_t &string, bool is_number) {
        if (string == "define") {
            return Token::DEFINE;
        } else if (string == "lambda") {
            return Token::LAMBDA;
        } else if (string == "eq?") {
            return Token::EQ;
        } else if (string == "cond") {
            return Token::COND;
        } else if (string == "(") {
            return Token::LPAREN;
        } else if (string == ")") {
            return Token::RPAREN;
        } else if (is_number) {
            return Token::NUMBER;
        } else {
            return Token::ATOM;
        }
    }
}
#endif //LISP_SIMULATOR_TOKEN_H
