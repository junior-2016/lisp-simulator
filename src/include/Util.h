//
// Created by junior on 19-5-9.
//

#ifndef LISP_SIMULATOR_UTIL_H
#define LISP_SIMULATOR_UTIL_H

#include "lisp.h"

namespace lisp {
    // 使用C++的常量名称空间,这样表示"string"s常量是std::string而不是const char*
    using namespace std::literals;

    using string_t = std::string;
    using string_ptr = std::unique_ptr<string_t>;
    using int_t = int32_t;
    using double_t = double;
    using char_t = char;
    using istream_t = std::istream;
    using ostream_t = std::ostream;
    using ifstream_t = std::ifstream;
    using ofstream_t = std::ofstream;
    using bool_t = struct bool_t {
        bool value;

        bool_t &operator=(const bool &v) {
            this->value = v;
            return *this;
        }

        bool_t &operator=(const bool_t &b) = default;

        friend ostream_t &operator<<(ostream_t &out, bool_t &a) {
            out << std::boolalpha << a.value;
            return out;
        }
    };

    template<typename T, typename ...Args>
    std::enable_if_t<!std::is_array<T>::value, std::unique_ptr<T>>
    make_ptr(Args &&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    /**
     * 创建大小为size的数组类型的unique_ptr.
     * 这里要求数组类型 T = type[] ,即维度 std::extent<T> = std::extent<type[]> 为 0,即空数组.
     */
    template<typename T>
    std::enable_if_t<std::is_array<T>::value && std::extent<T>::value == 0,
            std::unique_ptr<T>>
    make_ptr(size_t size) {
        return std::make_unique<T>(size);
    }

    namespace UtilTestCase {
        void test() {
            decltype(auto) string_ptr = make_ptr<string_t>("hello world"s);
            decltype(auto) bool_ptr = make_ptr<bool_t[]>(4);
            bool_ptr[0] = true;
            std::cout << (*string_ptr) << "\n";
            for (size_t i = 0; i < 4; i++) {
                std::cout << bool_ptr[i] << "\n";
            }
        }
    }
}

#endif //LISP_SIMULATOR_UTIL_H
