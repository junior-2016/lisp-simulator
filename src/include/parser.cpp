//
// Created by junior on 19-5-11.
//
#include "parser.h"

namespace lisp {
    inline void report_syntax_error(const string_t &string) {
        ExceptionHandle::global_handle().add_exception(ExceptionType::SYNTAX_ERROR, string);
    }

    std::vector<Token>::iterator end_pos;
    std::vector<Token>::iterator pos;

    inline bool is_token_end() { return pos == end_pos; }

    inline Token getNextToken() {
        Token token = std::move(*pos);
        ++pos;
        return token;
    }

    Ast::ptr traverseTokenList() {
        if (is_token_end()) {
            report_syntax_error("error");
            return nullptr;
        }
        Token token = getNextToken();
        switch (token.type) {
            case TokenType::ATOM:
            case TokenType::NUMBER:
            case TokenType::DEFINE:
            case TokenType::LAMBDA:
                return make_ptr<Ast>(std::move(token));

            case TokenType::LPAREN:

                break;
            default:
                report_syntax_error("");
        }
    }

    Ast::ptr parse(const string_t &source) {
        auto tokenList = getTokenList(source);
        pos = tokenList.begin();
        end_pos = tokenList.end();
        return traverseTokenList();
    }
}