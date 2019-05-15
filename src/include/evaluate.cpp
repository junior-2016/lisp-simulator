//
// Created by junior on 19-5-11.
//
#include "evaluate.h"

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
    auto eval(const Ast::ptr &root, const Env::handle &env) -> Function::Value {
        if (root->token != nullptr) { // 只有单独一个节点,没有任何child
            if (root->token->type == TokenType::NUMBER) {
                return Number(std::get<number_t>(root->token->value));
            } else {
                return env->find(*std::get<string_ptr>(root->token->value));
            }
        } else { // token为空,遍历child节点,往下处理
            if (root->children.empty()) { // token,child都没有,返回空值
                return nil();
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
                                env->insert(*std::get<string_ptr>(args[0]->token->value), eval(args[1], env));
                            } else {
                                report_semantic_error("error");
                            }
                            return nil();
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
                                            return nil();
                                        }
                                    }
                                    return Procedure(args[1], args_names, env); // 返回lambda定义的函数
                                } else {
                                    report_semantic_error("error");
                                    return nil();
                                }
                            } else {
                                report_semantic_error("error");
                                return nil();
                            }
                        }//else if (name == "cond") {} // 先不处理cond
                        else {
                            // op_child args 是 (atom args) 函数调用
                            auto function = eval(op_child, env);// 这里递归下去其实只要一步就能从env->find(op_child->token_name)得到函数
                            if (is_Value_Function(function)) {
                                std::vector<Function::Value> args_values;
                                args_values.reserve(args.size());
                                for (auto &arg:args) {
                                    args_values.push_back(eval(arg, env));
                                }
                                return std::get<Function>(function)(args_values);
                            } else {
                                report_semantic_error("error");
                                return nil();
                            }
                        }
                    } else { // 如果是一个数值,需要报错,因为数值不是函数调用
                        report_semantic_error("error");
                        return nil(); // 返回空值
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
                        return std::get<Function>(function)(args_values);
                    } else {
                        report_semantic_error("error");
                        return nil();
                    }
                }
            }
        }
    }
}
