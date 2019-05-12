//
// Created by junior on 19-5-11.
//
#include "parser.h"

namespace lisp {
    /**
     * 改进:
     * 给 Ast::ptr 加上新的 Type 来强化区分(不过好像不用....)
     * ATOM
     * NUMBER
     * COND_CALL
     * CALL
     * FUNCTION_ATOM(函数名)
     * FUNCTION_BODY(函数体)
     * ATOM_ARGUMENT_LIST (函数形参列表         => size()>=1[parser搞定] )
     * VALUE_ARGUMENT_LIST(call 调用的实参列表  => size()>=1[parser搞定] )
     * COND_ARGUMENT_LIST (cond_call 调用的参数 => size()>=2[parser搞定] && 至少一个参数的第一个列表返回true [analyser或者evaluate搞定])
     *
     */
    std::vector<Token>::iterator end;
    std::vector<Token>::iterator pos;

    inline void report_syntax_error(const string_t &string) {
        ExceptionHandle::global_handle().add_exception(ExceptionType::SYNTAX_ERROR, string);
    }

    inline bool is_token_end() { return pos == end; }

    template<typename TokenNoToTheEndProc>
    inline void check_token_to_end(TokenNoToTheEndProc proc) {
        if (is_token_end()) report_syntax_error("token to the end");
        else {
            proc();
        }
    }

    inline void getNextToken() { ++pos; }

    inline void match(TokenType target) {
        if (!is_token_end() && (*pos).type == target) {
            getNextToken();
        } else {
            report_syntax_error("match error");
        }
    }

    /* parser function */
    Ast::ptr parse_atom_argument_list();

    Ast::ptr parse_argument();

    Ast::ptr parse_argument_list();

    Ast::ptr parse_cond_argument();

    Ast::ptr parse_cond_argument_list();

    Ast::ptr parse_lambda_expression();

    Ast::ptr parse_call();

    Ast::ptr parse_definition_with_paren();

    Ast::ptr parse_definition();

    Ast::ptr parse_define();

    Ast::ptr parse_top_level_with_paren();

    Ast::ptr parse_top_level();

    /**
     * lambda_expr 参数
     *          => atom {atom}*
     */
    Ast::ptr parse_atom_argument_list() {
        Ast::ptr root = nullptr;
        check_token_to_end([&]() {
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
                    report_syntax_error("parse_atom_argument_list() error");
                    break;
            }
        });
        return root;
    }

    /**
     * argument =>
     *          atom |
     *          number |
     *          ( call )
     */
    Ast::ptr parse_argument() {
        Ast::ptr root = nullptr;
        check_token_to_end([&]() {
            switch ((*pos).type) {
                case TokenType::ATOM:
                case TokenType::NUMBER:
                    root = make_ptr<Ast>(std::move(*pos));
                    match((*pos).type);
                    break;
                case TokenType::LPAREN:
                    match(TokenType::LPAREN);
                    root = parse_call();
                    match(TokenType::RPAREN);
                    break;
                default:
                    report_syntax_error("parse_argument() error");
                    break;
            }
        });
        return root;
    }

/**
 * argument_list =>
 *           argument { argument }* [ 继续推导的条件: atom | number | '(' ]
 */
    Ast::ptr parse_argument_list() {
        Ast::ptr root = parse_argument();
        while (!is_token_end() &&
               ((*pos).type == TokenType::ATOM
                || (*pos).type == TokenType::NUMBER
                || (*pos).type == TokenType::LPAREN)) {
            root->add_child(parse_argument());
        }
        return root;
    }

/**
 * cond_argument =>
 *       ( cond_bool_arg cond_value_arg )
 *       parser 将 bool_arg 和 value_arg 都当做普通的argument,用 parse_argument() 来解析(类型需要在语义分析时得到)
 */
    Ast::ptr parse_cond_argument() {
        Ast::ptr root = nullptr;
        check_token_to_end([&]() {
            switch ((*pos).type) {
                case TokenType::LPAREN:
                    match(TokenType::LPAREN);
                    root = parse_argument(); // bool_arg
                    root->add_child(parse_argument()); // value_arg
                    match(TokenType::RPAREN);
                    break;
                default:
                    report_syntax_error("parse_cond_argument() error");
                    break;
            }
        });
        return root;
    }

/**
 * cond_argument_list =>
 *       { cond_argument } (2+)
 */
    Ast::ptr parse_cond_argument_list() {
        Ast::ptr root = parse_cond_argument();
        root->add_child(parse_cond_argument()); // 至少两个参数
        while (!is_token_end() && ((*pos).type == TokenType::LPAREN)) {
            root->add_child(parse_cond_argument());
        }
        return root;
    }

/**
 * lambda_expr =>
 *       lambda (atom_list) (call)
 */
    Ast::ptr parse_lambda_expression() {
        Ast::ptr root = nullptr;
        check_token_to_end([&]() {
            switch ((*pos).type) {
                case TokenType::LAMBDA:
                    root = make_ptr<Ast>(std::move(*pos));
                    match(TokenType::LAMBDA);
                    if (!is_token_end() && (*pos).type == TokenType::LPAREN) {
                        match(TokenType::LPAREN);
                        root->add_child(parse_atom_argument_list());
                        match(TokenType::RPAREN);
                        if (!is_token_end() && (*pos).type == TokenType::LPAREN) {
                            match(TokenType::LPAREN);
                            root->add_child(parse_call());
                            match(TokenType::RPAREN);
                        } else {
                            report_syntax_error("parse_lambda_expression() error");
                        }
                    } else {
                        report_syntax_error("parse_lambda_expression() error");
                    }
                    break;
                default:
                    report_syntax_error("parse_lambda_expression() error");
                    break;
            }
        });
        return root;
    }

/**
 * call =>
 *      atom 实参list |
 *      (lambda_expr) 实参list |
 *      cond cond_参数列表
 */
    Ast::ptr parse_call() {
        Ast::ptr root = nullptr;
        check_token_to_end([&]() {
            switch ((*pos).type) {
                case TokenType::ATOM:
                    root = make_ptr<Ast>(std::move(*pos));
                    match(TokenType::ATOM);
                    root->add_child(parse_argument_list());
                    break;
                case TokenType::LPAREN:
                    match(TokenType::LPAREN);
                    root = parse_lambda_expression();
                    match(TokenType::RPAREN);
                    if (root != nullptr) {
                        root->add_child(parse_argument_list());
                    }
                    break;
                case TokenType::COND:
                    root = make_ptr<Ast>(std::move(*pos));
                    match(TokenType::COND);
                    root->add_child(parse_cond_argument_list());
                    break;
                default:
                    report_syntax_error("parse_call() error");
                    break;
            }
        });
        return root;
    }

/**
  * definition_with_paren =>
  *         if token == '(' or atom or cond => call
  *         if token == lambda => lambda_expr
  */
    Ast::ptr parse_definition_with_paren() {
        Ast::ptr root = nullptr;
        check_token_to_end([&]() {
            switch ((*pos).type) {
                case TokenType::ATOM:
                case TokenType::LPAREN:
                case TokenType::COND:
                    root = parse_call();
                    break;
                case TokenType::LAMBDA:
                    root = parse_lambda_expression();
                    break;
                default:
                    report_syntax_error("parse_definition_with_paren() error");
                    break;
            }
        });
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
        check_token_to_end([&]() {
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
                    report_syntax_error("parse_definition() error");
                    break;
            }
        });
        return root;
    }

/**
 * define =>
 *       define atom definition
 */
    Ast::ptr parse_define() {
        Ast::ptr root = nullptr;
        check_token_to_end([&]() {
            switch ((*pos).type) {
                case TokenType::DEFINE:
                    root = make_ptr<Ast>(std::move(*pos));
                    match(TokenType::DEFINE);
                    if (!is_token_end() && (*pos).type == TokenType::ATOM) {
                        root->add_child(make_ptr<Ast>(std::move(*pos)));
                        match(TokenType::ATOM);
                        root->add_child(parse_definition());
                    } else {
                        report_syntax_error("parse_define() error");
                    }
                    break;
                default:
                    report_syntax_error("parse_define() error");
                    break;
            }
        });
        return root;
    }

/**
 * top_level_with_paren =>
 *       parse_define |
 *       parse_call
 */
    Ast::ptr parse_top_level_with_paren() {
        Ast::ptr root = nullptr;
        check_token_to_end([&]() {
            switch ((*pos).type) {
                case TokenType::DEFINE:
                    root = parse_define();
                    break;
                case TokenType::ATOM:       // atom argument_list
                case TokenType::LPAREN:     // (lambda_expr) argument_list
                case TokenType::COND :      // cond cond_arg_list
                    root = parse_call();
                    break;
                default:
                    report_syntax_error("parse_top_level_with_paren() error");
                    break;
            }
        });
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
        check_token_to_end([&]() {
            switch ((*pos).type) {
                case TokenType::LPAREN:
                    match(TokenType::LPAREN);
                    root = parse_top_level_with_paren();
                    match(TokenType::RPAREN);
                    break;
                case TokenType::ATOM:
                case TokenType::NUMBER:
                    root = make_ptr<Ast>(std::move(*pos));
                    match((*pos).type);
                    break;
                default:
                    report_syntax_error("parse_top_level() error");
                    break;
            }
        });
        return root;
    }

    Ast::ptr parse(const string_t &source) {
        auto tokenList = getTokenList(source);
        pos = tokenList.begin();
        end = tokenList.end();
        auto root = parse_top_level();
        if (!is_token_end()) {
            report_syntax_error("parser not to the end finally");
        }
        return root;
    }

}