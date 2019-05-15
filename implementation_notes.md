### C++实现细节:
- 函数接口放在.h文件,函数实现放在.cpp文件. 非特化的模板函数除外,非特化模板函数的接口和实现都要放在.h文件里,不能分离,
而特化的模板函数和普通函数一样,接口与定义需要分离.

- 对std::variant< A,B,C,.. >中所有类型共有的函数common_func进行接口提取,参考:
[https://riptutorial.com/cplusplus/example/18605/create-pseudo-method-pointers],
```c++
/**
 * @tparam F 的形式必须如下:
 * decltype(auto) F (auto && self, auto && ... args) {
 *      return decltype(self)(self).some_function_names(decltype(args)(args)...);
 * }
 */
template<typename F>
struct method_invoke {
    F f;

    method_invoke(F &&f) : f(std::move(f)) {}

    // Variant v;                 => 必须是std::variant类型的调用者
    // v->*(method_invoke_object) => v调用operator->*(..)返回一个函数
    // 然后就可以:  (v->*(method_invoke_object))(args) 进行函数调用
    template<typename Variant>
    friend decltype(auto) operator->*(Variant &variant, method_invoke &invoke) {
        // 这里用auto&&...做参数,用decltype(auto)做返回值,避免了对参数和返回值类型的限制,
        return [&](auto &&... args) -> decltype(auto) { // operator->*返回一个函数
            return std::visit(
                    [&](auto &&self) -> decltype(auto) { // 这里的self参数实际传递为std::visit的第二个参数variant
                        return invoke.f(decltype(self)(self), decltype(args)(args)...);
                    },
                    std::forward<Variant>(variant));
        };
    }
};

struct A {
    int do_something(int a) {
        return a;
    }
};

struct B {
    int do_something(int b) {
        return b;
    }
};

int main() {
    method_invoke do_something_invoke = ([](auto &&self, auto &&... args) -> decltype(auto) {
        return decltype(self)(self).do_something(decltype(args)(args)...);
    });
    std::variant<A, B> A_, B_;
    A_ = A{};
    B_ = B{};
    std::cout << (A_->*(do_something_invoke))(1) << "\n";
    std::cout << (B_->*(do_something_invoke))(2) << "\n";
}
最后输出:
1
2
```