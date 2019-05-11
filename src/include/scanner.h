//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_SCANNER_H
#define LISP_SIMULATOR_SCANNER_H

#include "lisp.h"
#include "util.h"
#include "token.h"

namespace lisp {
    inline void addToTokenList(const string_t &string, std::vector<Token> &tokenList) {
        TokenType type = getTokenByString(string);
        if (type == TokenType::NUMBER) {
            tokenList.push_back({type, get_number<number_t>(string)});
        } else {
            tokenList.push_back({type, make_ptr<string_t>(string)});
        }
    }

    decltype(auto) getTokenList(string_t source) {
        std::vector<Token> tokenList;
        boost::replace_all(source, "(", " ( ");
        boost::replace_all(source, ")", " ) ");
        std::regex rgx("\\s+");
        std::sregex_token_iterator iter(source.begin(), source.end(), rgx, -1);
        std::sregex_token_iterator end;
        if (*iter != "(") {
            // 如果开始的第一个token不是"("的话,只能将第一个token作为scanner的结果.
            // (当代码是想单独输出一个variable时需要这样处理)
            addToTokenList(*iter, tokenList);
        } else {
            for (; iter != end; ++iter) {
                addToTokenList(*iter, tokenList);
            }
        }
        return tokenList;
    }
}
#endif //LISP_SIMULATOR_SCANNER_H
