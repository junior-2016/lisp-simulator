//
// Created by junior on 19-5-11.
//

#include "token.h"

namespace lisp {
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