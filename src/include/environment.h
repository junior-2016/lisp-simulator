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
#include "type_system.h"

namespace lisp {
    class Env {
    public:
        using handle = Ptr<Env>::Type;
    private:
        std::unordered_map<string_t, Function::Value> map;
        Env::handle outer_env = nullptr;
    public:
        Env() = default; // 这个构造函数主要是给构造全局Env使用
        Env(const std::vector<string_t> &args_names, const std::vector<Function::Value> &args_value,
            Env::handle outer_env);

        bool insert(const string_t &name, const Function::Value &value);

        Function::Value update(const string_t &atom_name, const Function::Value &update_value);

        Function::Value find(const string_t &atom_name);

        static Env::handle global_env();
    };
}
#endif //LISP_SIMULATOR_ENVIRONMENT_H
