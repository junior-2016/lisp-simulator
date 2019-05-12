//
// Created by junior on 19-5-11.
//
#include "util.h"

namespace lisp {
    bool is_number(const string_t &string) {
        std::regex regex(R"([+-]?((([0-9]*[.])?[0-9]+)|(([0-9]+[.])[0-9]*)))");
        return std::regex_match(string, regex) && !string.empty();
    }

    bool is_integer(const string_t &string) {
        std::regex regex(R"([+-]?(0|[1-9]+))");
        return std::regex_match(string, regex) && !string.empty();
    }

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
            // 正确的数值表示
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

            // 错误的数值表示
            std::cout << std::boolalpha << is_number(".") << "\n";
            std::cout << std::boolalpha << is_number("+") << "\n";
            std::cout << std::boolalpha << is_number("-") << "\n";
            std::cout << std::boolalpha << is_number("") << "\n";
            std::cout << std::boolalpha << is_number("++0.23") << "\n";
            std::cout << std::boolalpha << is_number("abc+12") << "\n";

            decltype(auto) string_ptr = make_ptr<string_t>("hello world"s);
            std::cout << (*string_ptr) << "\n";
        }
    }
}
