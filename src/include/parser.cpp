//
// Created by junior on 19-5-11.
//
#include "parser.h"
#include "scanner.h"
#include "exception.h"

namespace lisp {
    std::vector<Token>::const_iterator end;
    std::vector<Token>::iterator pos;

    inline bool is_token_end() { return pos == end; }

    inline void getNextToken() { ++pos; }

    inline void report_syntax_error(const string_t &string) {
        ExceptionHandle::global_handle().add_exception(ExceptionType::SYNTAX_ERROR, string);
    }

    inline void match(TokenType target) {
        if (!is_token_end() && (*pos).type == target) {
            getNextToken();
        } else {
            report_syntax_error("");
        }
    }

    /* parser function */
    Ast::ptr parse_formal_argument_list();

    Ast::ptr parse_actual_argument_list();

    Ast::ptr parse_cond_argument_list();

    Ast::ptr parse_lambda_expression();

    Ast::ptr parse_call();

    Ast::ptr parse_definition_with_paren();

    Ast::ptr parse_definition();

    Ast::ptr parse_define();

    Ast::ptr parse_cond();

    Ast::ptr parse_first_level();

    Ast::ptr parse_top_level();


    /**
     * 形参list
     *       => atom {atom}*
     */
    Ast::ptr parse_formal_argument_list() {
        Ast::ptr root = nullptr;
        if (!is_token_end()) {
            switch ((*pos).type) {
                case TokenType::ATOM :
                    root = make_ptr<Ast>(std::move(*pos));
                    match(TokenType::ATOM);
                    while (!is_token_end() && (*pos).type == TokenType::ATOM) {
                        root->add_child(make_ptr<Ast>(std::move(*pos)));
                        match(TokenType::ATOM);
                    }
                    break;

                default:
                    report_syntax_error("");
                    break;
            }
        }
        return root;
    }

    /**
     * 实参list, 可以是 [atom, call, lambda_call, number] 返回的任何值
     *       =>
     *
     */
    Ast::ptr parse_actual_argument_list() {
        Ast::ptr root = nullptr;

        return root;
    }

    /**
     * lambda_expr =>
     *       lambda (形参list) (call)
     */
    Ast::ptr parse_lambda_expression() {
        Ast::ptr root = nullptr;
        if (!is_token_end()) {
            switch ((*pos).type) {
                case TokenType::LAMBDA:
                    root = make_ptr<Ast>(std::move(*pos));
                    match(TokenType::LAMBDA);
                    if (!is_token_end() && (*pos).type == TokenType::LPAREN) {
                        match(TokenType::LPAREN);
                        root->add_child(parse_formal_argument_list());
                        match(TokenType::RPAREN);
                        if (!is_token_end() && (*pos).type == TokenType::LPAREN) {
                            match(TokenType::LPAREN);
                            root->add_child(parse_call());
                            match(TokenType::RPAREN);
                        } else {
                            report_syntax_error("");
                        }
                    } else {
                        report_syntax_error("");
                    }
                    break;

                default:
                    report_syntax_error("");
                    break;
            }
        }
        return root;
    }

    /**
     * call =>
     *      atom 实参list |
     *      (lambda_expr) 实参list
     */
    Ast::ptr parse_call() {
        Ast::ptr root = nullptr;
        if (!is_token_end()) {
            switch ((*pos).type) {
                case TokenType::ATOM:
                    root = make_ptr<Ast>(std::move(*pos));
                    match(TokenType::ATOM);
                    root->add_child(parse_actual_argument_list());
                    break;

                case TokenType::LPAREN:
                    match(TokenType::LPAREN);
                    root = parse_lambda_expression();
                    match(TokenType::RPAREN);
                    if (root != nullptr) {
                        root->add_child(parse_actual_argument_list());
                    }
                    break;

                default:
                    report_syntax_error("");
                    break;
            }
        }
        return root;
    }

    /**
      * definition_with_paren =>
      *         if token == '(' or atom => call
      *         if token == lambda => lambda_expr
      */
    Ast::ptr parse_definition_with_paren() {
        Ast::ptr root = nullptr;
        if (!is_token_end()) {
            switch ((*pos).type) {
                case TokenType::ATOM:
                case TokenType::LPAREN:
                    root = parse_call();
                    break;

                case TokenType::LAMBDA:
                    root = parse_lambda_expression();
                    break;

                default:
                    report_syntax_error("");
                    break;
            }
        }
        return root;
    }

    /**
     * definition =>
     *         atom |
     *         number |
     *         ( definition_with_paren )
     */
    Ast::ptr parse_definition() {
        Ast::ptr root = nullptr;
        if (!is_token_end()) {
            switch ((*pos).type) {
                case TokenType::ATOM:
                case TokenType::NUMBER:
                    root = make_ptr<Ast>(std::move(*pos));
                    match((*pos).type);
                    break;

                case TokenType::LPAREN:
                    match(TokenType::LPAREN);
                    root = parse_definition_with_paren();
                    match(TokenType::RPAREN);
                    break;

                default:
                    report_syntax_error("");
                    break;
            }
        }
        return root;
    }

    /**
     * define =>
     *       define atom definition
     */
    Ast::ptr parse_define() {
        Ast::ptr root = nullptr;
        if (!is_token_end()) {
            switch ((*pos).type) {
                case TokenType::DEFINE:
                    root = make_ptr<Ast>(std::move(*pos));
                    match(TokenType::DEFINE);
                    if (!is_token_end() && (*pos).type == TokenType::ATOM) {
                        root->add_child(make_ptr<Ast>(std::move(*pos)));
                        match(TokenType::ATOM);
                        root->add_child(parse_definition());
                    } else {
                        report_syntax_error("");
                    }
                    break;

                default:
                    report_syntax_error("");
                    break;
            }
        }
        return root;
    }

    Ast::ptr parse_cond_argument_list() {
        Ast::ptr root = nullptr;

        return root;
    }

    Ast::ptr parse_cond() {
        Ast::ptr root = nullptr;

        return root;
    }

    /**
     * first_token =>
     *       define ... |
     *       atom   ... |
     *       cond   ...
     */
    Ast::ptr parse_first_level() {
        Ast::ptr root = nullptr;
        if (!is_token_end()) {
            switch ((*pos).type) {
                case TokenType::DEFINE:
                    root = parse_define();
                    break;

                case TokenType::ATOM:       // atom 实参list
                case TokenType::LPAREN:     // (lambda_expr) 实参list
                    root = parse_call();
                    break;

                case TokenType::COND:
                    root = parse_cond();
                    break;

                default:
                    report_syntax_error("");
                    break;
            }
        }
        return root;
    }

    /**
     * top_level =>
     *      atom    |
     *      number  |
     *      ( ... ) |
     */
    Ast::ptr parse_top_level() {
        Ast::ptr root = nullptr;
        if (!is_token_end()) {
            switch ((*pos).type) {
                case TokenType::LPAREN:
                    match(TokenType::LPAREN);
                    root = parse_first_level();
                    match(TokenType::RPAREN);
                    break;

                case TokenType::ATOM:
                case TokenType::NUMBER:
                    root = make_ptr<Ast>(std::move(*pos));
                    match((*pos).type);
                    break;

                default:
                    report_syntax_error("");
                    break;
            }
        }
        return root;
    }

    Ast::ptr parse(const string_t &source) {
        auto tokenList = getTokenList(source);
        pos = tokenList.begin();
        end = tokenList.end();
        auto root = parse_top_level();
        if (!is_token_end()) {
            report_syntax_error("parser not to the end.");
        }
        return root;
    }
}