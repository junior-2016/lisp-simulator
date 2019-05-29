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

    Function::Value try_assign_Value(Function::Value &a, const Function::Value &b) {
        if (is_Value_const(a) || !is_Value_same_type(a, b)) {
            return nil::null();
        }
        if (is_Value_Number(a)) {
            std::get<Number::ptr>(a)->number = std::get<Number::ptr>(b)->number;
        } else if (is_Value_Bool(a)) {
            std::get<Bool::ptr>(a)->value = std::get<Bool::ptr>(b)->value;
        }
        // Function 或者 nil 类型的变量被禁止修改const属性,所以这里无需检查Function或nil
        // (因为它们必定是const的,在一开始就被过滤了)
        /*else if (is_Value_nil(a) || is_Value_Function(a)) {
            return nil::null();
        }*/
        return a;
    }
}