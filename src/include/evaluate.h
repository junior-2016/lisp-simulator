//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_EVALUATE_H
#define LISP_SIMULATOR_EVALUATE_H

#include "lisp.h"
#include "util.h"
#include "parser.h"
#include "environment.h"

namespace lisp {
    /**
     * (define ID value):
     * 不能重绑定, 内部已经默认绑定了 +,-,*,/ 四个标识符, 所以如果写 (define + 5) 就会报variable + unbound错误.
     * 又比如: 已经写 (define add 5), 如果再写 (define add 4) 就会报 variable add unbound 错误.
     * 并且注意到 lisp 的 variable 是不允许 unbound 的,即每一个 ID 只能绑定一个数值/function,并且绑定后不能再修改.
     */
    /**
     * 如果解析像 () 这样的空表达式, 生成的Ast是nullptr, 那么 evaluate 解析返回 "nil".
     * evaluate的时候要注意 => 有一些 ast::ptr 是 nullptr, 要跳过.
     */

    /**
     * 非csp的evaluator
     */
    void eval(Ast::ptr root, Env env) {

    }

    /**
     * csp的evaluator
     */
}
#endif //LISP_SIMULATOR_EVALUATE_H
