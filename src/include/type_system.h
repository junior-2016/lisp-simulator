//
// Created by junior on 19-5-16.
//

#ifndef LISP_SIMULATOR_TYPE_SYSTEM_H
#define LISP_SIMULATOR_TYPE_SYSTEM_H

#include "lisp.h"
#include "util.h"

namespace lisp {

    struct TypeSystem {
    protected:
        bool is_const = true; // 所有类型默认const
    public:
        explicit TypeSystem(bool is_const = true) : is_const(is_const) {}

        virtual inline string_t to_string() const {
            return "";
        }

        virtual ~TypeSystem() = default;

        bool is_type_const() const { return is_const; }

        void set_type_var() { is_const = false; }
    };

    struct Number : public TypeSystem { // 数值类型
        using ptr = Ptr<Number>::Type;
        number_t number;

        explicit Number(number_t number, bool is_const = true) :
                TypeSystem(is_const), number(number) {}

        inline string_t to_string() const override {
            return std::to_string(number);
        }

        ~Number() override = default;
    };

    struct Bool : public TypeSystem { // 布尔类型
        using ptr = Ptr<Bool>::Type;
        bool value;

        explicit Bool(bool value, bool is_const = true) :
                TypeSystem(is_const), value(value) {}

        inline string_t to_string() const override {
            return value ? "True" : "False";
        }
    };

    struct nil : TypeSystem { // 空类型,必然是const型
        using ptr = Ptr<nil>::Type;

        nil() : TypeSystem(true) {}

        inline string_t to_string() const override { return "nil"; }

        static nil::ptr null() {
            static nil::ptr global_nil = make_ptr<nil>();
            return global_nil;
        }
    };

    /**
     * 定义函数抽象类型 Function,同时带有虚函数接口: virtual Value operator()(std::vector<Value>&args) ;
     */
    class Function : public TypeSystem {
    public:
        using ptr = Ptr<Function>::Type;
        const static size_t Function_Value_Nil_Index = 0;
        const static size_t Function_Value_Number_Index = 1;
        const static size_t Function_Value_Bool_Index = 2;
        const static size_t Function_Value_Function_Index = 3;
        using Value = std::variant<nil::ptr, Number::ptr, Bool::ptr, Function::ptr>;
        using FunctionType = std::function<Function::Value(const std::vector<Function::Value> &)>;
    private:
        FunctionType function = [](const std::vector<Function::Value> &) -> Function::Value { return nil::null(); };
        // function初始化为一个默认的空实现,也就是直接返回空值
    public:
        // 继承Function的函数类有时候想要通过重写operator()来实现一些别的功能,并且也用不到内部持有的function对象(比如Procedure类),
        // 但继承的函数类还是需要初始化父类的成员,所以这里提供一个无参的Function()构造来初始化内部function对象为默认的空实现.
        explicit Function(bool is_const = true) : TypeSystem(is_const) {}

        explicit Function(FunctionType function, bool is_const = true) :
                TypeSystem(is_const), function(std::move(function)) {}

        ~Function() override = default;

        // 默认的operator()直接执行内部持有的functional对象
        virtual Function::Value operator()(const std::vector<Function::Value> &args) {
            return function(args);
        }

        // 提供一个输出函数类型信息的虚函数
        string_t to_string() const override {
            return "Function";
        }
    };

    inline bool is_Value_nil(const Function::Value &value) {
        return value.index() == Function::Function_Value_Nil_Index;
    }

    inline bool is_Value_Number(const Function::Value &value) {
        return value.index() == Function::Function_Value_Number_Index;
    }

    inline bool is_Value_Bool(const Function::Value &value) {
        return value.index() == Function::Function_Value_Bool_Index;
    }

    inline bool is_Value_Function(const Function::Value &value) {
        return value.index() == Function::Function_Value_Function_Index;
    }

    // 判断两个Value是否同类型
    inline bool is_Value_same_type(const Function::Value &a, const Function::Value &b) {
        return (is_Value_Number(a) && is_Value_Number(b)) ||
               (is_Value_Bool(a) && is_Value_Bool(b)) ||
               (is_Value_nil(a) && is_Value_nil(b)) ||
               (is_Value_Function(a) && is_Value_Function(b));
    }

    // 为了方便调用Function::Value的公共接口(在TypeSystem定义),提供一个variant_method_invoke模板,借助std::visit调用
    template<typename F>
    struct variant_method_invoke {
        F f;

        variant_method_invoke(F &&f) : f(std::move(f)) {}

        template<typename Variant>
        friend decltype(auto) operator->*(Variant &variant, variant_method_invoke &invoke) {
            return [&](auto &&... args) -> decltype(auto) { // operator->*返回一个函数
                return std::visit(
                        [&](auto &&self) -> decltype(auto) {
                            // 这里的self参数实际传递为std::visit的第二个参数variant
                            return invoke.f(decltype(self)(self), decltype(args)(args)...);
                        },
                        std::forward<Variant>(variant));
            };
        }
    };

    string_t value_to_string(Function::Value value);

    bool is_Value_const(Function::Value value);

    void set_Value_var(Function::Value value);
}
#endif //LISP_SIMULATOR_TYPE_SYSTEM_H
