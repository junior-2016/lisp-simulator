//
// Created by junior on 19-5-16.
//
#include "type_system.h"

namespace lisp {
    variant_method_invoke variant_to_string_invoke = [](auto &&self, auto &&... args) -> decltype(auto) {
        return decltype(self)(self)->to_string(decltype(args)(args)...);
    };

    variant_method_invoke variant_is_value_const_invoke = [](auto &&self, auto &&... args) -> decltype(auto) {
        return decltype(self)(self)->is_type_const(decltype(args)(args)...);
    };

    variant_method_invoke variant_set_value_var_invoke = [](auto &&self, auto &&... args) -> decltype(auto) {
        decltype(self)(self)->set_type_var(decltype(args)(args)...);
    };

    string_t value_to_string(Function::Value value) {
        return (value->*(variant_to_string_invoke))();
    }

    bool is_Value_const(Function::Value value) {
        return (value->*(variant_is_value_const_invoke))();
    }

    void set_Value_var(Function::Value value) {
        (value->*(variant_set_value_var_invoke))();
    }

}