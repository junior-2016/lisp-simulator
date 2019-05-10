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

    using string_t = std::string;
    using string_ptr = std::unique_ptr<string_t>;

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

    /*
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
    };*/

    template<typename T, typename ...Args>
    std::enable_if_t<!std::is_array<T>::value, std::unique_ptr<T>>
    make_ptr(Args &&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

/**
 * 创建大小为size的数组类型的unique_ptr.
 * 这里要求数组类型 T = type[], 此时维度 std::extent<T> = std::extent<type[]> 为 0,即空数组.
 * eg: decltype(auto) ptr = make_ptr<int[]>(4);
 */
    template<typename T>
    std::enable_if_t<std::is_array<T>::value && std::extent<T>::value == 0,
            std::unique_ptr<T>>
    make_ptr(size_t size) {
        return std::make_unique<T>(size);
    }

/**
 * 用正则表达式判断字符串是否为数值.
 */
    bool is_number(const string_t &string) {
        std::regex regex(R"([+-]?((([0-9]*[.])?[0-9]+)|(([0-9]+[.])?[0-9]*)))");
        return std::regex_match(string, regex) && !string.empty();
    }

    bool is_integer(const string_t &string) {
        std::regex regex(R"([+-]?(0|[1-9]+))");
        return std::regex_match(string, regex) && !string.empty();
    }

    template<typename T>
    T get_number(const string_t &string);

    template<>
    int_t get_number(const string_t &string) {
        return std::stoi(string);
    }

    template<>
    double_t get_number(const string_t &string) {
        return std::stod(string);
    }

    template<>
    float_t get_number(const string_t &string) {
        return std::stof(string);
    }

    namespace UtilTestCase {
        void test() {
            std::cout << std::boolalpha << is_number("0.3") << "\n";
            std::cout << std::boolalpha << is_number("0.") << "\n";
            std::cout << std::boolalpha << is_number(".3") << "\n";
            std::cout << std::boolalpha << is_number("+0.3") << "\n";
            std::cout << std::boolalpha << is_number("-0.3") << "\n";
            std::cout << std::boolalpha << is_number("+3.") << "\n";
            std::cout << std::boolalpha << is_number("-3.") << "\n";
            std::cout << std::boolalpha << is_number("+.0") << "\n";
            std::cout << std::boolalpha << is_number("-.0") << "\n";

            std::cout << std::boolalpha << is_number("+3") << "\n";
            std::cout << std::boolalpha << is_number("-2") << "\n";
            std::cout << std::boolalpha << is_number("2") << "\n";

            std::cout << std::boolalpha << is_number(".") << "\n";
            std::cout << std::boolalpha << is_number("") << "\n";
            std::cout << std::boolalpha << is_number("++0.23") << "\n";
            std::cout << std::boolalpha << is_number("abc+12") << "\n";

            decltype(auto) string_ptr = make_ptr<string_t>("hello world"s);
            std::cout << (*string_ptr) << "\n";
        }
    }
}

#endif //LISP_SIMULATOR_UTIL_H
