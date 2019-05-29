//
// Created by junior on 19-5-15.
//

#include "environment.h"

namespace lisp {

    // 公开的Env构造接口
    Env::Env(const std::vector<string_t> &args_names, const std::vector<Function::Value> &args_value,
             Env::handle outer_env) : outer_env(std::move(outer_env)) {
        if (args_names.size() == args_value.size()) {
            for (size_t i = 0; i < args_names.size(); i++) {
                // 这里不需要对args_value为nil()的做报错处理. 因为即使传递给函数的参数是nil(),
                // 函数在执行时必然会检查参数是否满足自己的要求,如果不满足就继续返回nil(),
                // 这样一层层传递下去,最后整个表达式返回的也是nil(). 符合我们计算错误返回nil()的语义要求,是绝佳的设计.
                map.insert({args_names[i], args_value[i]});
            }
        } else {
            report_semantic_error("Env(args_names,args_values,outer_env) args_names ans args_value not same size");
        }
    }

    bool Env::insert(const string_t &name, const Function::Value &value) {
        // 插入的同时返回是否插入成功,如果插入失败,可能是前面已经有符号定义了,不能重复定义; 也可能是插入的值value为空
        if (is_Value_nil(value)) return false;
        return this->map.insert({name, value}).second;
    }

    // 修改某个atom的值,和find一样可能需要在outer_env里查找.
    // 1. 修改时,若atom没有查到定义,返回nil;
    // 2. 若查到定义,但atom之前储存的值类型与更新的值类型不同,或者atom储存的值是const类型,不能修改,也返回nil;
    // (这里不用检查update_value是nil的情况,因为前面insert()已经确保了map里储存的都是非nil的值,通过第二点就可以排除)
    Function::Value Env::update(const string_t &atom_name, const Function::Value &update_value) {
        auto pos = this->map.find(atom_name);
        if (pos == this->map.end()) {
            if (this->outer_env == nullptr) {
                return nil::null();
            } else {
                return this->outer_env->update(atom_name, update_value);
            }
        }
        // 使用安全的assign操作!!!
        return try_assign_Value((*pos).second, update_value);
    }

    Function::Value Env::find(const string_t &atom_name) {
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
    Env::handle Env::global_env() {
        static Env::handle global_handle = make_ptr<Env>(); // outer_env 为空
        // 注意全局env里所有预定义的变量都是const的.
        global_handle->map = {
                {"+",
                          make_ptr<Function>(
                                  [](const std::vector<Function::Value> &args) -> Function::Value {
                                      if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                          return make_ptr<Number>(std::get<Number::ptr>(args[0])->number +
                                                                  std::get<Number::ptr>(args[1])->number);
                                      report_semantic_error("+ compute error");
                                      return nil::null();
                                  }, true)
                },
                {"-",
                          make_ptr<Function>(
                                  [](const std::vector<Function::Value> &args) -> Function::Value {
                                      if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                          return make_ptr<Number>(std::get<Number::ptr>(args[0])->number -
                                                                  std::get<Number::ptr>(args[1])->number);
                                      report_semantic_error("- compute error");
                                      return nil::null();
                                  }, true)
                },
                {"*",
                          make_ptr<Function>(
                                  [](const std::vector<Function::Value> &args) -> Function::Value {
                                      if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1]))
                                          return make_ptr<Number>(std::get<Number::ptr>(args[0])->number *
                                                                  std::get<Number::ptr>(args[1])->number);
                                      report_semantic_error("* compute error");
                                      return nil::null();
                                  }, true)
                },
                {"/",
                          make_ptr<Function>(
                                  [](const std::vector<Function::Value> &args) -> Function::Value {
                                      if (args.size() == 2 && is_Value_Number(args[0]) && is_Value_Number(args[1])
                                          && std::get<Number::ptr>(args[1])->number != 0) { // 注意除法检查
                                          return make_ptr<Number>(std::get<Number::ptr>(args[0])->number /
                                                                  std::get<Number::ptr>(args[1])->number);
                                      }
                                      report_semantic_error("/ compute error");
                                      return nil::null();
                                  }, true)
                },
                {"eq?",
                          make_ptr<Function>(
                                  [](const std::vector<Function::Value> &args) -> Function::Value {
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
                                  }, true)
                },
                {"True",  make_ptr<Bool>(true)},
                {"False", make_ptr<Bool>(false)}
        };
        return global_handle;
    }
}