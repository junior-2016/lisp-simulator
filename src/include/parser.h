//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_PARSER_H
#define LISP_SIMULATOR_PARSER_H

#include "lisp.h"
#include "util.h"
#include "token.h"
#include "scanner.h"
#include "exception.h"

namespace lisp {
    struct Ast {
        using ptr = Ptr<Ast>::Type;
        std::vector<Ast::ptr> children;
        Token::ptr token = nullptr;

        explicit Ast(Token::ptr token) : token(std::move(token)) {}

        explicit Ast() = default;

        void add_child(const Ast::ptr &ptr) {
            if (ptr != nullptr) { // 确保添加的child不为空
                children.push_back(ptr);
            }
        }

        static string_t to_string(const Ast::ptr& ptr, int tab = 0);
    };

    Ast::ptr parse(const string_t &source);
}
#endif //LISP_SIMULATOR_PARSER_H
