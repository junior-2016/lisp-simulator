//
// Created by junior on 19-5-15.
//

#include "environment.h"

namespace lisp {
    variant_method_invoke variant_to_string_invoke = [](auto &&self, auto &&... args) -> decltype(auto) {
        return decltype(self)(self).to_string(decltype(args)(args)...);
    };

    string_t value_to_string(Function::Value value) {
        return (value->*(variant_to_string_invoke))();
    }
}