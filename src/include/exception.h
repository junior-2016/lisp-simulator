//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_EXCEPTION_H
#define LISP_SIMULATOR_EXCEPTION_H

#include "lisp.h"
#include "util.h"

namespace lisp {
    /**
     * 全局异常收集
     */

    enum class ExceptionType {
        SYNTAX_ERROR,
        SEMANTIC_ERROR
    };

    string_t printExceptionType(ExceptionType type) {
        switch (type) {
            case ExceptionType::SYNTAX_ERROR :
                return "SYNTAX_ERROR";
            case ExceptionType::SEMANTIC_ERROR:
                return "SEMANTIC_ERROR";
            default:
                return "Unknown_Error";
        }
    }

    struct ExceptionEntry {
        string_t message;
        ExceptionType type;
    };

    // 使用单例模式
    class ExceptionHandle {
    private:
        std::vector<ExceptionEntry> errors;

        ExceptionHandle() = default;

    public:
        static ExceptionHandle &global_handle() {
            static ExceptionHandle handle;
            return handle;
        }

    public:
        ExceptionHandle(ExceptionHandle const &) = delete;

        void operator=(ExceptionHandle const &) = delete;

        void add_exception(ExceptionType type, const string_t &message) {
            errors.push_back(ExceptionEntry{message, type});
        }

        friend std::ostream &operator<<(std::ostream &out, const ExceptionHandle &handle) {
            out << "\t ExceptionType \t ExceptionMessage \n";
            for (auto &exception:handle.errors) {
                out << "\t " << printExceptionType(exception.type) << " \t "
                    << exception.message << " \n";
            }
            return out;
        }

        bool hasException() const {
            return !errors.empty();
        }
    };
}
#endif //LISP_SIMULATOR_EXCEPTION_H
