//
// Created by junior on 19-5-11.
//
#include "parser.h"

namespace lisp {
    std::vector<Token::ptr>::iterator end_pos;
    std::vector<Token::ptr>::iterator pos;

    inline bool is_token_end() { return pos == end_pos; }

    inline void skip_token() { ++pos; }

    int list_deep = 0; // 用来判断 define/defvar 有没有位于顶层

    Ast::ptr traverseTokenList() {
        if (is_token_end()) {
            report_syntax_error("parse to the end early");
            return nullptr;
        }
        auto token = *pos;
        skip_token();
        if (token->type == TokenType::RPAREN) {
            report_syntax_error("paren match error");
            return nullptr;
        } else if (token->type == TokenType::LPAREN) {
            Ast::ptr root = make_ptr<Ast>();
            list_deep++;
            while ((*pos)->type != TokenType::RPAREN) {
                root->add_child(traverseTokenList());
            }
            list_deep--;
            skip_token();
            return root;
        } else {
            if ((token->type == TokenType::DEFINE || token->type == TokenType::DEFVAR)
                && list_deep != 1) {
                report_syntax_error("define or defvar not on the top level");
            }
            return make_ptr<Ast>(token);
        }
    }

    Ast::ptr parse(const string_t &source) {
        auto tokenList = Scanner::getTokenList(source);
        pos = tokenList.begin();
        end_pos = tokenList.end();
        list_deep = 0;
        return traverseTokenList();
    }

    string_t Ast::to_string(const Ast::ptr &ptr, int tab) {
        string_t result;
        for (int i = 0; i < tab; i++) {
            result += "\t";
        }
        if (ptr->token != nullptr) {
            if (ptr->token->type == TokenType::NUMBER) {
                result += std::to_string(std::get<number_t>(ptr->token->value));
            } else {
                result += *std::get<string_ptr>(ptr->token->value);
            }
        } else {
            result += "[\n";
            for (auto &item : ptr->children) {
                result += to_string(item, tab + 1);
            }
            for (int i = 0; i < tab; i++) {
                result += "\t";
            }
            result += "]";
        }
        return result += "\n";
    }
}