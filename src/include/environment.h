#include <utility>

//
// Created by junior on 19-5-12.
//

#ifndef LISP_SIMULATOR_ENVIRONMENT_H
#define LISP_SIMULATOR_ENVIRONMENT_H

#include "lisp.h"
#include "util.h"
#include "parser.h"
#include "exception.h"

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
     * 注意 除法函数不能除以0 ...
     */
    inline void report_semantic_error(const string_t &string) {
        ExceptionHandle::global_handle().add_exception(ExceptionType::SEMANTIC_ERROR, string);
    }

    enum class Type {
        Null,   // 空值
        Number, // 数值
        Func,   // 函数
        Boolean // 布尔值
    };
    enum class ArgType {

    };

    struct Number { // 数值类型
        number_t number;

        explicit Number(number_t number) : number(number) {}

        inline string_t to_string() const {
            return std::to_string(number);
        }
    };

    struct Bool { // 布尔类型
        bool value;

        explicit Bool(bool value) : value(value) {}

        inline string_t to_string() const {
            return value ? "True" : "False";
        }
    };

    struct nil { // 空类型
        inline string_t to_string() const {
            return "nil";
        }
    };

    using Value = std::variant<nil, Number, Bool>;

    template<typename T>
    bool is_nil(T value) {
        return std::is_same<decltype(value), nil>::value;
    }

    inline bool is_Value_nil(Value value) {
        return value.index() == 0;
    }

    inline bool is_Value_Number(Value value) {
        return value.index() == 1;
    }

    inline bool is_Value_Bool(Value value) {
        return value.index() == 2;
    }

    using Function = std::function<Value(std::vector<Value> &)>; // 现在还有一个缺陷是参数的类型,是不是应该抽象出一个ArgType

    struct EnvValue {
        Type type; // TODO 这个Type可能需要简化为 Type::Func 和 Type::Value 两个就行, 剩下的靠Value.index()自己就可以判断了
        std::variant<Value, Function> value;
    };

    class Env {
    private:
        std::unordered_map<string_t, EnvValue> map;
        Env outer_env;
    public:
        Env() = default;

        Env(const std::vector<string_t> &args_names, const std::vector<Value> &args_value,
            const Env &outer_env) : outer_env(outer_env) {
            if (args_names.size() == args_value.size()) {
                for (size_t i = 0; i < args_names.size(); i++) {
                    // 这里不需要对args_value为nil()的做报错处理. 因为即使传递给函数的参数是nil(),
                    // 函数在执行时必然会检查参数是否满足自己的要求,如果不满足就继续返回nil(),
                    // 这样一层层传递下去,最后整个表达式返回的也是nil(). 符合我们计算错误返回nil()的语义要求,是绝佳的设计.
                    if (is_Value_Number(args_value[i])) {
                        map.insert({args_names[i], EnvValue{Type::Number, args_value[i]}});
                    } else if (is_Value_Bool(args_value[i])) {
                        map.insert({args_names[i], EnvValue{Type::Boolean, args_value[i]}});
                    } else if (is_Value_nil(args_value[i])) {
                        map.insert({args_names[i], EnvValue{Type::Null, args_value[i]}});
                    }
                }
            } else {
                report_semantic_error("error");
            }
        }

        static Env global_env() {
            static Env env;
            env.map = {
                    {"+",
                              EnvValue{Type::Func, (Function) ([](std::vector<Value> &args) -> Value {
                                  if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                      return Number(std::get<Number>(args[0]).number +
                                                    std::get<Number>(args[1]).number);
                                  report_semantic_error("+ compute error");
                                  return nil();
                              })}
                    },
                    {"-",
                              EnvValue{Type::Func, (Function) ([](std::vector<Value> &args) -> Value {
                                  if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                      return (Number) (std::get<Number>(args[0]).number -
                                                       std::get<Number>(args[1]).number);
                                  report_semantic_error("- compute error");
                                  return nil();
                              })}
                    },
                    {"*",
                              EnvValue{Type::Func, (Function) ([](std::vector<Value> &args) -> Value {
                                  if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                      return (Number) (std::get<Number>(args[0]).number *
                                                       std::get<Number>(args[1]).number);
                                  report_semantic_error("* compute error");
                                  return nil();
                              })}
                    },
                    {"/",
                              EnvValue{Type::Func, (Function) ([](std::vector<Value> &args) -> Value {
                                  if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1])
                                      && std::get<Number>(args[1]).number != 0) { // 注意除法检查
                                      return (Number) (std::get<Number>(args[0]).number /
                                                       std::get<Number>(args[1]).number);
                                  }
                                  report_semantic_error("/ compute error");
                                  return nil();
                              })}
                    },
                    {"True",  EnvValue{Type::Boolean, Bool(true)}},
                    {"False", EnvValue{Type::Boolean, Bool(false)}}
            };
            return env;
        }

        EnvValue find(const string_t &atom_name) {
            auto pos = map.find(atom_name);
            if (pos == map.end()) {
                return {Type::Null, nil()};
            } else {

            }
        }
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
    private:
        // lambda 函数体就是: Ast[ 0: lambda 1: (args_list) 2: (body)] 切下来的 Ast[2],直接保存这个节点即可,
        // 后面再通过eval(body,env)来进一步执行body的内容,即懒惰执行.
        Ast::ptr function_body;

        // lambda 函数体的参数列表,直接保存其原子名称即可.
        std::vector<string_t> function_args_names;

        // lambda函数当前所处的 Environment.
        // 当调用 Procedure().operator(args)的时候,这个env又作为函数体里面另一个内部嵌套函数的 outer_environment
        Env env;
    public:
        Procedure(Ast::ptr body, const std::vector<string_t> &args_names, Env env) {
            this->function_body = std::move(body);
            this->function_args_names = args_names;
            this->env = env;
        }

        Value operator()(const std::vector<Value> &args) {
            eval(this->function_body, Env(this->function_args_names, args, this->env));
        }

    };
}
#endif //LISP_SIMULATOR_ENVIRONMENT_H
