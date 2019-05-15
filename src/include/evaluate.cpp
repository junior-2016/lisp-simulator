//
// Created by junior on 19-5-11.
//
#include "evaluate.h"

namespace lisp {
    /**
     * 辅助函数,尝试cast到procedure::ptr,需要判断返回的指针是否为空
     */
    Procedure::ptr try_cast_to_procedure_function(const Function::ptr &func) {
        return std::dynamic_pointer_cast<Procedure>(func);
        // 或者 return dynamic_cast<Procedure*>(func.get()) != nullptr;
    }

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
    auto eval(const Ast::ptr &root, const Env::handle &env) -> Function::Value {
        if (root->token != nullptr) { // 只有单独一个节点,没有任何child
            if (root->token->type == TokenType::NUMBER) {
                return make_ptr<Number>(std::get<number_t>(root->token->value));
            } else {
                return env->find(*std::get<string_ptr>(root->token->value));
            }
        } else { // token为空,遍历child节点,往下处理
            if (root->children.empty()) { // token,child都没有,返回空值
                return nil::null();
            } else {
                auto op_child = root->children[0]; // 取第一个child作为op
                std::vector<Ast::ptr> args(root->children.begin() + 1, root->children.end());
                if (op_child->token != nullptr) {
                    // op_child 是 单独一个 Token,此时肯定是: atom函数调用/define定义/cond调用/lambda定义
                    if (op_child->token->type == TokenType::ATOM) {
                        string_t name = *std::get<string_ptr>(op_child->token->value);
                        if (name == "define") {
                            // define 结束后是没有返回的,但是这里可以考虑返回nil
                            // define 必须满足: define atom [...]
                            if (args.size() == 2 &&
                                args[0]->token != nullptr &&
                                args[0]->token->type == TokenType::ATOM) {
                                if (!env->insert(*std::get<string_ptr>(args[0]->token->value), eval(args[1], env))) {
                                    report_semantic_error("redefine error"); // 如果插入后返回false,说明符号重定义了
                                }
                            } else {
                                report_semantic_error("define error");
                            }
                            return nil::null();
                        } else if (name == "lambda") {
                            // lambda [0]: args_names [1]: body
                            if (args.size() == 2) {
                                std::vector<string_t> args_names; // 处理lambda函数参数名列表
                                if (!args[0]->children.empty()) {
                                    for (auto &child:args[0]->children) {
                                        if (child->token != nullptr && child->token->type == TokenType::ATOM) {
                                            args_names.push_back(*std::get<string_ptr>(child->token->value));
                                        } else {
                                            report_semantic_error("error");
                                            return nil::null();
                                        }
                                    }
                                    return make_ptr<Procedure>(args[1], args_names, env); // 返回lambda定义的函数
                                } else {
                                    report_semantic_error("error");
                                    return nil::null();
                                }
                            } else {
                                report_semantic_error("error");
                                return nil::null();
                            }
                        } else if (name == "cond") {
                            if (args.size() >= 2) {
                                // cond的args至少有两个,每一个args都是(bool_expr other_expr)的形式,
                                // 并且至少有一个bool_expr返回true.
                                size_t true_expr_pos = args.size();
                                for (size_t i = 0; i < args.size(); i++) {
                                    if (args[i]->children.size() != 2) {
                                        report_semantic_error("error");
                                        return nil::null();
                                    }
                                    auto bool_expr = eval(args[i]->children[0], env); // bool_expr
                                    if (!is_Value_Bool(bool_expr)) { // 如果不是Bool类型,报错
                                        report_semantic_error("error");
                                        return nil::null();
                                    }
                                    if (true_expr_pos == args.size() && std::get<Bool::ptr>(bool_expr)->value) {
                                        // 找到第一个bool_expr为true的,然后设置位置.
                                        // 后面的参数即使出现true也不管了,所以这里多一个条件是true_expr_pos没有被修改过.
                                        true_expr_pos = i;
                                    }
                                }
                                if (true_expr_pos == args.size()) {
                                    report_semantic_error("error"); // 没有一个参数的bool expr是true,报错
                                    return nil::null();
                                } else {
                                    return eval(args[true_expr_pos]->children[1], env);// 最后才计算唯一正确的参数的返回值
                                }
                            } else {
                                report_semantic_error("error");
                                return nil::null();
                            }
                        } else {
                            // op_child args 是 (atom args) 函数调用
                            auto function = eval(op_child, env);// 这里递归下去其实只要一步就能从env->find(op_child->token_name)得到函数
                            if (is_Value_Function(function)) {
                                std::vector<Function::Value> args_values;
                                args_values.reserve(args.size());
                                for (auto &arg:args) {
                                    args_values.push_back(eval(arg, env));
                                }
                                auto proc = try_cast_to_procedure_function(std::get<Function::ptr>(function));
                                if (proc != nullptr) {
                                    return proc->operator()(args_values);
                                } else {
                                    return std::get<Function::ptr>(function)->operator()(args_values);
                                }
                            } else {
                                report_semantic_error("error");
                                return nil::null();
                            }
                        }
                    } else { // 如果是一个数值,需要报错,因为数值不是函数调用
                        report_semantic_error("error");
                        return nil::null(); // 返回空值
                    }
                } else {
                    // op_child 本身也是一个需要运行的函数调用,同时返回一个函数(用Procedure继承Function来实现这个功能)
                    // 比如：((f args) args),第一个op_child(f args)需要调用并返回一个新函数
                    // 但是写法和前面的函数调用写法是一样的
                    auto function = eval(op_child, env);// 这里递归下去可能还需要计算很多次才能返回一个函数
                    if (is_Value_Function(function)) {
                        std::vector<Function::Value> args_values;
                        args_values.reserve(args.size());
                        for (auto &arg:args) {
                            args_values.push_back(eval(arg, env));
                        }
                        auto proc = try_cast_to_procedure_function(std::get<Function::ptr>(function));
                        if (proc != nullptr) {
                            return proc->operator()(args_values);
                        } else {
                            return std::get<Function::ptr>(function)->operator()(args_values);
                        }
                    } else {
                        report_semantic_error("error");
                        return nil::null();
                    }
                }
            }
        }
    }
}
