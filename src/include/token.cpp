//
// Created by junior on 19-5-11.
//

#include "token.h"

namespace lisp {
    TokenType getTokenByString(const string_t &string) {
        if (string == "(") {
            return TokenType::LPAREN;
        } else if (string == ")") {
            return TokenType::RPAREN;
        } else if (string == "lambda") {
            return TokenType::LAMBDA;
        } else if (string == "cond") {
            return TokenType::COND;
        } else if (string == "define") {
            return TokenType::DEFINE;
        } else if (string == "defvar") {
            return TokenType::DEFVAR;
        } else if (string == "set!") {
            return TokenType::SETVAR;
        } else if (is_number(string)) {
            return TokenType::NUMBER;
        } else {
            return TokenType::ATOM;
        }
    }
}