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
    // 非csp的eval
    auto eval(const Ast::ptr& root, const Env::handle &env) -> Function::Value;

    // TODO : csp的evaluator

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
    // Procedure作为Function的子类,可以直接存在EnvValue里(向上直接cast为父类Function)
    class Procedure : public Function {
    private:
        // lambda 函数体就是: Ast[ 0: lambda 1: (args_list) 2: (body)] 切下来的 Ast[2],直接保存这个节点即可,
        // 后面再通过eval(body,env)来进一步执行body的内容,即懒惰执行.
        Ast::ptr function_body;

        // lambda 函数体的参数列表,直接保存其原子名称即可.
        std::vector<string_t> function_args_names;

        // lambda函数当前所处的 Environment.
        // 当调用 Procedure().operator(args)的时候,这个env又作为函数体里面另一个内部嵌套函数的 outer_environment
        Env::handle env;
    public:
        Procedure(const Ast::ptr &body, const std::vector<string_t> &args_names, const Env::handle &env) : Function() {
            this->function_body = body;
            this->function_args_names = args_names;
            this->env = env;
        }

        // 覆盖Function的operator()实现
        Value operator()(const std::vector<Value> &args) override {
            return eval(this->function_body, make_ptr<Env>(this->function_args_names, args, this->env));
        }

        string_t to_string() const override {
            return Function::to_string(); // 这里懒得写,直接返回父类实现
        }

        ~Procedure() override = default;
    };
}
#endif //LISP_SIMULATOR_EVALUATE_H
