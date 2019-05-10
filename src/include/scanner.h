//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_SCANNER_H
#define LISP_SIMULATOR_SCANNER_H

#include "lisp.h"
#include "util.h"
#include "token.h"

namespace lisp {
    decltype(auto) getTokenList(string_t source) {
        std::vector<TokenRet> tokenList;
        boost::replace_all(source, "(", " ( ");
        boost::replace_all(source, ")", " ) ");
        std::cout << source;
        std::regex rgx("\\s+");
        std::sregex_token_iterator iter(source.begin(), source.end(), rgx, -1);
        std::sregex_token_iterator end;
        Token token;
        number_t value;
        for (; iter != end; ++iter) {
            if (*iter == "define") {
                token = Token::DEFINE;
            } else if (*iter == "lambda") {
                token = Token::LAMBDA;
            } else if (*iter == "eq?") {
                token = Token::EQ;
            } else if (*iter == "cond") {
                token = Token::COND;
            } else if (*iter == "(") {
                token = Token::LPAREN;
            } else if (*iter == ")") {
                token = Token::RPAREN;
            } else if (is_number(*iter)) {
                token = Token::NUMBER;
                value = get_number<number_t>(*iter);
            } else {
                token = Token::ATOM;
            }
            if (token != Token::NUMBER) {
                tokenList.push_back({token, make_ptr<string_t>(*iter)});
            } else {
                tokenList.push_back({token, value});
            }
        }
        return tokenList;
    }
}
#endif //LISP_SIMULATOR_SCANNER_H
