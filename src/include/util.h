//
// Created by junior on 19-5-9.
//

#ifndef LISP_SIMULATOR_UTIL_H
#define LISP_SIMULATOR_UTIL_H

#include "lisp.h"

namespace lisp {
    // 使用C++的常量名称空间,这样表示"string"s常量是std::string而不是const char*
    using namespace std::literals;
    using namespace std::chrono_literals;
    using namespace std::placeholders;

    template<typename T>
    struct Ptr {
        using Type = std::shared_ptr<T>;
    };

    using string_t = std::string;
    using string_ptr = Ptr<string_t>::Type;

    template<typename T, typename ...Args>
    std::enable_if_t<!std::is_array<T>::value, std::shared_ptr<T>>
    make_ptr(Args &&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    std::enable_if_t<std::is_array<T>::value && std::extent<T>::value == 0,
            std::shared_ptr<T>>
    make_ptr(size_t size) {
        return std::make_shared<T>(size);
    }

    using char_t = char;
    using int_t = int32_t;
    using float_t = float;
    using double_t = double;
#ifdef USE_INT_AS_NUMBER
    using number_t = int_t;
#elif defined USE_DOUBLE_AS_NUMBER
    using number_t = double_t;
#elif defined USE_FLOAT_AS_NUMBER
    using number_t = float_t;
#endif

    using sstream_t = std::stringstream;
    using istream_t = std::istream;
    using ostream_t = std::ostream;
    using ifstream_t = std::ifstream;
    using ofstream_t = std::ofstream;

/**
 * 用正则表达式判断字符串是否为数值.
 */
    bool is_number(const string_t &string);

    bool is_integer(const string_t &string);

    template<typename T>
    T get_number(const string_t &string);

    template<>
    int_t get_number(const string_t &string);

    template<>
    double_t get_number(const string_t &string);

    template<>
    float_t get_number(const string_t &string);

    namespace UtilTestCase {
        void test();
    }

}

#endif //LISP_SIMULATOR_UTIL_H
