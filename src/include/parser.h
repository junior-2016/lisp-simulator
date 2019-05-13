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
    class Ast {
    public:
        using ptr = std::unique_ptr<Ast>;
    private:
        std::vector<Ast::ptr> children;
        std::unique_ptr<Token> token = nullptr;
    public:
        explicit Ast(std::unique_ptr<Token> token) : token(std::move(token)) {}

        explicit Ast() = default;

        void add_child(Ast::ptr ptr) {
            if (ptr != nullptr) { // 确保添加的child不为空
                children.push_back(std::move(ptr));
            }
        }

        static string_t to_string(Ast::ptr ptr, int tab = 0);
    };

    Ast::ptr parse(const string_t &source);
}
#endif //LISP_SIMULATOR_PARSER_H
