#include <utility>

//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_PARSER_H
#define LISP_SIMULATOR_PARSER_H

#include "lisp.h"
#include "util.h"
#include "token.h"
#include "symbol_table.h"

namespace lisp {
    /**
     * (define ID value):
     * 不能重绑定, 内部已经默认绑定了 +,-,*,/ 四个标识符, 所以如果写 (define + 5) 就会报variable + unbound错误.
     * 又比如: 已经写 (define add 5), 如果再写 (define add 4) 就会报 variable add unbound 错误.
     * 并且注意到 lisp 的 variable 是不允许 unbound 的,即每一个 ID 只能绑定一个数值/function,并且绑定后不能再修改.
     */
    class Ast {
        /**
         * 用多重嵌套链表实现 Ast.
         */

    };

    Ast parse(const std::vector<TokenRet> &tokenList) {
        return Ast();
    }
}
#endif //LISP_SIMULATOR_PARSER_H
