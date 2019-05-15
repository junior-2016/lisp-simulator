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
        using ptr = Ptr<Number>::Type;
        number_t number;

        explicit Number(number_t number) : number(number) {}

        inline string_t to_string() const {
            return std::to_string(number);
        }
    };

    struct Bool { // 布尔类型
        using ptr = Ptr<Bool>::Type;
        bool value;

        explicit Bool(bool value) : value(value) {}

        inline string_t to_string() const {
            return value ? "True" : "False";
        }
    };

    // TODO 将nil改成单例模式
    struct nil { // 空类型
        using ptr = Ptr<nil>::Type;

        inline string_t to_string() const {
            return "nil";
        }

    public:
        static nil::ptr null() {
            static nil::ptr p = make_ptr<nil>();
            return p;
        }
    };

    /**
     * 定义函数抽象类型 Function,同时带有虚函数接口: virtual Value operator()(std::vector<Value>&args) ;
     */
    class Function {
    public:
        // Value的定义应该放在Function里,从而依赖于Function. 因为Function的参数和返回值都是Value类型,
        // 并且Value自己也需要拥有Function的定义,那么最佳的设计就是将Value变成Function的一个成员类型.
        // 注意Value在储存Function的时候必须储存函数的指针,这样后面才能通过指针cast来判断这个函数是不是特定的函数子类对象!!
        using ptr = Ptr<Function>::Type;
        using Value = std::variant<nil::ptr, Number::ptr, Bool::ptr, Function::ptr>;
        using FunctionType = std::function<Function::Value(const std::vector<Function::Value> &)>;
    private:
        FunctionType function = [](const std::vector<Function::Value> &) -> Function::Value { return nil::null(); };
        // function初始化为一个默认的空实现,也就是直接返回空值
    public:
        // 继承Function的函数类有时候想要通过重写operator()来实现一些别的功能,并且也用不到内部持有的function对象(比如Procedure类),
        // 但继承的函数类还是需要初始化父类的成员,所以这里提供一个无参的Function()构造来初始化内部function对象为默认的空实现.
        Function() = default;

        explicit Function(FunctionType function) : function(std::move(function)) {}

        virtual ~Function() = default;

        // 默认的operator()直接执行内部持有的functional对象
        virtual Function::Value operator()(const std::vector<Function::Value> &args) {
            return function(args);
        }

        // 提供一个输出函数类型信息的虚函数
        virtual string_t to_string() const {
            return "Function";
        }
    };

    inline bool is_Value_nil(const Function::Value &value) {
        return value.index() == 0;
    }

    inline bool is_Value_Number(const Function::Value &value) {
        return value.index() == 1;
    }

    inline bool is_Value_Bool(const Function::Value &value) {
        return value.index() == 2;
    }

    inline bool is_Value_Function(const Function::Value &value) {
        return value.index() == 3;
    }

    // 判断两个Value是否同类型
    inline bool is_Value_same_type(const Function::Value &a, const Function::Value &b) {
        return (is_Value_Number(a) && is_Value_Number(b)) ||
               (is_Value_Bool(a) && is_Value_Bool(b)) ||
               (is_Value_nil(a) && is_Value_nil(b)) ||
               (is_Value_Function(a) && is_Value_Function(b));
    }

    // 统一输出接口
    template<typename F>
    struct variant_method_invoke {
        F f;

        variant_method_invoke(F &&f) : f(std::move(f)) {}

        template<typename Variant>
        friend decltype(auto) operator->*(Variant &variant, variant_method_invoke &invoke) {
            return [&](auto &&... args) -> decltype(auto) { // operator->*返回一个函数
                return std::visit(
                        [&](auto &&self) -> decltype(auto) {
                            // 这里的self参数实际传递为std::visit的第二个参数variant
                            return invoke.f(decltype(self)(self), decltype(args)(args)...);
                        },
                        std::forward<Variant>(variant));
            };
        }
    };

    string_t value_to_string(Function::Value value);

    class Env {
    public:
        using handle = Ptr<Env>::Type;
    private:
        std::unordered_map<string_t, Function::Value> map;
        Env::handle outer_env = nullptr;
    public:
        Env() = default; // 这个构造函数主要是给构造全局Env使用
        // 公开的Env构造接口
        Env(const std::vector<string_t> &args_names, const std::vector<Function::Value> &args_value,
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

        bool insert(const string_t &name, const Function::Value &value) {
            // 插入的同时返回是否插入成功,如果插入失败,说明前面已经有符号定义了,不能重复定义
            return this->map.insert({name, value}).second;
        }

        Function::Value find(const string_t &atom_name) {
            auto pos = this->map.find(atom_name);
            if (pos == this->map.end()) { // 当前env没有找到atom的定义,尝试向外层的env查找
                if (this->outer_env == nullptr) { // 当前env已经是最外层了
                    return nil::null();   // 只能直接返回空值
                } else {                                     // 当前env外层还有一个env
                    return this->outer_env->find(atom_name); // 向外层env查找
                }
            }
            return (*pos).second; // 在当前environment找到这个atom的定义
        }

        // 获取全局environment. 注意全局env的outer_env是nullptr,利用这一点来停止Env::find()的递归查找(当outer_env为空时停止查找)
        static Env::handle global_env() {
            static Env::handle global_handle = make_ptr<Env>(); // outer_env 为空
            global_handle->map = {
                    {"+",
                              make_ptr<Function>(
                                      Function([](const std::vector<Function::Value> &args) -> Function::Value {
                                          if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                              return make_ptr<Number>(std::get<Number::ptr>(args[0])->number +
                                                                      std::get<Number::ptr>(args[1])->number);
                                          report_semantic_error("+ compute error");
                                          return nil::null();
                                      }))
                    },
                    {"-",
                              make_ptr<Function>(
                                      Function([](const std::vector<Function::Value> &args) -> Function::Value {
                                          if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                              return make_ptr<Number>(std::get<Number::ptr>(args[0])->number -
                                                                      std::get<Number::ptr>(args[1])->number);
                                          report_semantic_error("- compute error");
                                          return nil::null();
                                      }))
                    },
                    {"*",
                              make_ptr<Function>(
                                      Function([](const std::vector<Function::Value> &args) -> Function::Value {
                                          if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                              return make_ptr<Number>(std::get<Number::ptr>(args[0])->number *
                                                                      std::get<Number::ptr>(args[1])->number);
                                          report_semantic_error("* compute error");
                                          return nil::null();
                                      }))
                    },
                    {"/",
                              make_ptr<Function>(
                                      Function([](const std::vector<Function::Value> &args) -> Function::Value {
                                          if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1])
                                              && std::get<Number::ptr>(args[1])->number != 0) { // 注意除法检查
                                              return make_ptr<Number>(std::get<Number::ptr>(args[0])->number /
                                                                      std::get<Number::ptr>(args[1])->number);
                                          }
                                          report_semantic_error("/ compute error");
                                          return nil::null();
                                      }))
                    },
                    {"eq?",
                              make_ptr<Function>(
                                      Function([](const std::vector<Function::Value> &args) -> Function::Value {
                                          // 两个Value类型相同就能参与比较
                                          if (args.size() == 2 && is_Value_same_type(args[0], args[1])) {
                                              if (is_Value_Number(args[0])) {
                                                  return make_ptr<Bool>(std::get<Number::ptr>(args[0])->number
                                                                        == std::get<Number::ptr>(args[1])->number);
                                              } else if (is_Value_Bool(args[0])) {
                                                  return make_ptr<Bool>(std::get<Bool::ptr>(args[0])->value
                                                                        == std::get<Bool::ptr>(args[1])->value);
                                              } else { // 两个nil或两个Function不可比较,所以返回nil
                                                  return nil::null();
                                              }
                                          }
                                          report_semantic_error("eq? compute error");
                                          return nil::null();
                                      }))
                    },
                    {"True",  make_ptr<Bool>(true)},
                    {"False", make_ptr<Bool>(false)}
            };
            return global_handle;
        }
    };
}
#endif //LISP_SIMULATOR_ENVIRONMENT_H
