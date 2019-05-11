//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_SCANNER_H
#define LISP_SIMULATOR_SCANNER_H

#include "lisp.h"
#include "util.h"
#include "token.h"

namespace lisp {
    inline void addToTokenList(const string_t &string, std::vector<Token> &tokenList);

    std::vector<Token> getTokenList(string_t source);
}
#endif //LISP_SIMULATOR_SCANNER_H
