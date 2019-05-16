//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_SCANNER_H
#define LISP_SIMULATOR_SCANNER_H

#include "lisp.h"
#include "util.h"
#include "token.h"

namespace lisp {
    // TODO 添加注释,字符串解析等功能..
    struct Scanner {
    private:
        static inline void addToTokenList(const string_t &string, std::vector<Token::ptr> &tokenList) {
            TokenType type = getTokenByString(string);
            if (type == TokenType::NUMBER) {
                tokenList.push_back(make_ptr<Token>(type, get_number<number_t>(string)));
            } else {
                tokenList.push_back(make_ptr<Token>(type, make_ptr<string_t>(string)));
            }
        }

    public:
        static std::vector<Token::ptr> getTokenList(string_t source) {
            std::vector<Token::ptr> tokenList;
            boost::replace_all(source, "(", " ( "); // 区分开 (
            boost::replace_all(source, ")", " ) "); // 区分开 )
            boost::algorithm::trim(source); // 注意最后去掉字符串前后的空格,否则后面分割token会出错
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
    };
}
#endif //LISP_SIMULATOR_SCANNER_H
