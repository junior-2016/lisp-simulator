//
// Created by junior on 19-5-12.
//

#ifndef LISP_SIMULATOR_ENVIRONMENT_H
#define LISP_SIMULATOR_ENVIRONMENT_H

#include "lisp.h"
#include "util.h"

namespace lisp {
    /**
     * 符号表预定义的Symbol(Atom)(注意这些Symbol已经预定义,所以不允许在代码中重新定义)
     * defined_symbol_map
     * [Atom(原子名称)  Type(类型)  Syntax(语义)=>值(如果是number/bool保存值,
     * 如果是func保存ast节点用于后面计算,如果是预定义的
     * func保存C++function指针或者匿名函数)]
     * [  True          Bool      true(C++基本类型)       ]
     * [  False         Bool      false                  ]
     * [   +            Fun       [](a,b)->{return a+b;} ]
     * [   -            Fun                              ]
     * [   *            Fun                              ]
     * [   /            Fun                              ]
     * [   %            Fun                              ]
     * 其他的还有: 操作符函数  > < >= <= == != && || ! & | ^
     *            逻辑函数    eq? cond
     *            控制流逻辑  if (test) (then_expr) (else_expr), while ....
     *            数学函数    sin cos tan atan ....
     *
     */
    class Env {

    };

    /**
     * 核心的要素在于重载的 T operator()(Args && args):
     * 后面声明 define V lambda (x y) ( lambda ( x y) (+ x y) ) 时,即:
     * Env[V] <--- Store ---  Procedure object <--- Construct --- Procedure(lambda_args_names, lambda_body)
     * 调用时: ( (V 5 6) 5 6 )
     * parser 结果 x = [ [V 5 6] 5 6]
     * 取出 xx = x[0] = [ V 5 6 ],
     * eval() 从 Env[V] 查出这个 Procedure 对象V,
     * 接着:
     * eval() 调用 V.operator(5,6),
     * 接着:
     * V.operator(5,6) 内部调用 eval(this->body,Env(this->args_names,args,this->env))
     */
    class Procedure {
        /*
    public:
        template<typename FunctionBody, typename FunctionArgs>
        Procedure(FunctionBody body, std::initializer_list<functionArgs> &args_names) {
            this->body = body;
            this->args_names = args_names;
        }

        std::vector<typename FunctionArgs> args_names;
        typename FunctionBody body;

        template<typename T, typename ... Args>
        T operator()(Args &&... args) {
            eval(this->body, Env(this->args_names, args, this->env))
        }
         */
    };
}
#endif //LISP_SIMULATOR_ENVIRONMENT_H
