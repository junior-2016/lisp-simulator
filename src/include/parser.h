#include <utility>

//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_PARSER_H
#define LISP_SIMULATOR_PARSER_H

#include "lisp.h"
#include "util.h"
#include "token.h"

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
            children.push_back(std::move(ptr));
        }

        const char *to_string() const {
            return nullptr;
        }
    };

    Ast::ptr parse(const string_t &source);

}
#endif //LISP_SIMULATOR_PARSER_H
