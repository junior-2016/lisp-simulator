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
        using ptr =  std::unique_ptr<Ast>;
    private:
        std::vector<Ast::ptr> children;
        Token token;
    public:
        explicit Ast(Token token) : token(std::move(token)) {}

        void add_child(Ast::ptr ptr) {
            // 这里确保添加的 child 不会是一个 nullptr.
            if (ptr != nullptr) {
                children.push_back(std::move(ptr));
            }
        }

        string_t to_string() const {
            return "";
        }
    };

    Ast::ptr parse(const string_t &source);

}
#endif //LISP_SIMULATOR_PARSER_H
