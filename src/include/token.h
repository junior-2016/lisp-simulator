#include <utility>

//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_TOKEN_H
#define LISP_SIMULATOR_TOKEN_H

#include "lisp.h"
#include "util.h"

namespace lisp {
    // define lambda cond 这些都放在 evaluate 处理..
    enum class TokenType {
        NUMBER = 'N', // 数值,支持整数模式或者浮点数模式
        ATOM = 'A',   // 除数值以外的其他原子
        LPAREN = '(', // 左括号
        RPAREN = ')'  // 右括号
    };

    struct Token {
        using ptr = Ptr<Token>::Type;
        TokenType type = TokenType::ATOM;
        std::variant<number_t, string_ptr> value;

        Token(TokenType type, std::variant<number_t, string_ptr> value) :
                type(type), value(std::move(value)) {}
    };

    TokenType getTokenByString(const string_t &string);
}
#endif //LISP_SIMULATOR_TOKEN_H
