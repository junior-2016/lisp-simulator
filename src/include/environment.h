#include <utility>

#include <utility>

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
    // TODO: environment和evaluate的接口对错误处理统一用了semantic,
    //  后面必须区分syntax和semantic,因为有一些是syntax的问题

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

    // 前置声明Function, 使得Value可以包含Function.
    // (由于Value需要包含Function,而Function又需要使用Value作为参数和返回值,所以这里的循环依赖需要通过前置声明来解决)
    class Function;

    using Value = std::variant<nil, Number, Bool, Function>;

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

    inline bool is_Value_Function(Value value) {
        return value.index() == 3;
    }

    // 判断两个Value是否同类型
    inline bool is_Value_same_type(Value a, Value b) {
        return (is_Value_Number(a) && is_Value_Number(b)) ||
               (is_Value_Bool(a) && is_Value_Bool(b)) ||
               (is_Value_nil(a) && is_Value_nil(b)) ||
               (is_Value_Function(a) && is_Value_Function(b));
    }

    /**
     * 定义函数抽象类型 Function,同时带有虚函数接口: virtual Value operator()(std::vector<Value>&args) ;
     */
    class Function {
    public:
        using FunctionType = std::function<Value(const std::vector<Value> &)>;
    private:
        FunctionType function = [](const std::vector<Value> &) -> Value { return nil(); };
        // function初始化为一个默认的空实现,也就是直接返回空值
    public:
        // 继承Function的函数类有时候想要通过重写operator()来实现一些别的功能,并且也用不到内部持有的function对象(比如Procedure类),
        // 但继承的函数类还是需要初始化父类的成员,所以这里提供一个无参的Function()构造来初始化内部function对象为默认的空实现.
        Function() = default;

        explicit Function(FunctionType function) : function(std::move(function)) {}

        virtual ~Function() = default;

        // 默认的operator()直接执行内部持有的functional对象
        virtual Value operator()(const std::vector<Value> &args) {
            return function(args);
        }

        // 提供一个输出函数类型信息的虚函数
        virtual string_t to_string() const {
            return "Function";
        }
    };

    // 统一输出接口
    string_t to_string(Value value) {
        if (is_Value_Number(value)) {
            return std::get<Number>(value).to_string();
        } else if (is_Value_nil(value)) {
            return std::get<nil>(value).to_string();
        } else if (is_Value_Bool(value)) {
            return std::get<Bool>(value).to_string();
        } else {
            return std::get<Function>(value).to_string();
        }
    }

    class Env {
    public:
        // Env::handle应该用shared_ptr而不是unique_ptr,因为会同时存在多个env,
        // 并且在构造Procedure时传递外界env后,外界env还是要使用的,应该设计成所有权共享的智能指针.
        using handle = std::shared_ptr<Env>;
    private:
        std::unordered_map<string_t, Value> map;
        Env::handle outer_env = nullptr;
    private:
        Env() = default; // 这个构造函数主要是给构造全局Env使用,所以用private修饰,外界不可使用.
    public:
        // 公开的Env构造接口
        Env(const std::vector<string_t> &args_names, const std::vector<Value> &args_value,
            Env::handle outer_env) : outer_env(std::move(outer_env)) {
            if (args_names.size() == args_value.size()) {
                for (size_t i = 0; i < args_names.size(); i++) {
                    // 这里不需要对args_value为nil()的做报错处理. 因为即使传递给函数的参数是nil(),
                    // 函数在执行时必然会检查参数是否满足自己的要求,如果不满足就继续返回nil(),
                    // 这样一层层传递下去,最后整个表达式返回的也是nil(). 符合我们计算错误返回nil()的语义要求,是绝佳的设计.
                    map.insert({args_names[i], args_value[i]});
                }
            } else {
                report_semantic_error("error");
            }
        }

        void insert(const string_t &name, Value value) {
            this->map.insert({name, value});
        }

        Value find(const string_t &atom_name) {
            auto pos = this->map.find(atom_name);
            if (pos == this->map.end()) { // 当前env没有找到atom的定义,尝试向外层的env查找
                if (this->outer_env == nullptr) { // 当前env已经是最外层了
                    return nil();   // 只能直接返回空值
                } else {                                     // 当前env外层还有一个env
                    return this->outer_env->find(atom_name); // 向外层env查找
                }
            }
            return (*pos).second; // 在当前environment找到这个atom的定义
        }

        // 获取全局environment. 注意全局env的outer_env是nullptr,利用这一点来停止Env::find()的递归查找(当outer_env为空时停止查找)
        static Env::handle global_env() {
            static Env::handle global_handle = std::make_shared<Env>(); // outer_env 为空
            global_handle->map = {
                    {"+",
                              Function([](const std::vector<Value> &args) -> Value {
                                  if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                      return Number(std::get<Number>(args[0]).number +
                                                    std::get<Number>(args[1]).number);
                                  report_semantic_error("+ compute error");
                                  return nil();
                              })
                    },
                    {"-",
                              Function([](const std::vector<Value> &args) -> Value {
                                  if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                      return (Number)(std::get<Number>(args[0]).number -
                                                      std::get<Number>(args[1]).number);
                                  report_semantic_error("- compute error");
                                  return nil();
                              })
                    },
                    {"*",
                              Function([](const std::vector<Value> &args) -> Value {
                                  if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                      return (Number)(std::get<Number>(args[0]).number *
                                                      std::get<Number>(args[1]).number);
                                  report_semantic_error("* compute error");
                                  return nil();
                              })
                    },
                    {"/",
                              Function([](const std::vector<Value> &args) -> Value {
                                  if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1])
                                      && std::get<Number>(args[1]).number != 0) { // 注意除法检查
                                      return (Number)(std::get<Number>(args[0]).number /
                                                      std::get<Number>(args[1]).number);
                                  }
                                  report_semantic_error("/ compute error");
                                  return nil();
                              })
                    },
                    {"eq?",
                              Function([](const std::vector<Value> &args) -> Value {
                                  // 两个Value类型相同就能参与比较
                                  if (args.size() == 2 && is_Value_same_type(args[0], args[1])) {
                                      if (is_Value_Number(args[0])) {
                                          return (Bool)(std::get<Number>(args[0]).number
                                                        == std::get<Number>(args[1]).number);
                                      } else if (is_Value_Bool(args[0])) {
                                          return (Bool)(std::get<Bool>(args[0]).value
                                                        == std::get<Bool>(args[1]).value);
                                      } else { // 两个nil或两个Function不可比较,所以返回nil
                                          return nil();
                                      }
                                  }
                                  report_semantic_error("eq? compute error");
                                  return nil();
                              })
                    },
                    {"True",  Bool(true)},
                    {"False", Bool(false)}
            };
            return global_handle;
        }
    };
}
#endif //LISP_SIMULATOR_ENVIRONMENT_H
