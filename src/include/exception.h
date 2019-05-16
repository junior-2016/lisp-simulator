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

    string_t printExceptionType(ExceptionType type);

    // 使用单例模式
    class ExceptionHandle {
    private:
        struct ExceptionEntry {
            string_t message;
            ExceptionType type;
        };
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

        string_t to_string() const {
            string_t string = "\t ExceptionType \t ExceptionMessage \n";
            for (auto &exception:errors) {
                string += ("\t " + printExceptionType(exception.type) + " \t "
                           + exception.message) + " \n";
            }
            return string;
        }

        bool hasException() const {
            return !errors.empty();
        }

        void clearException() {
            errors.clear();
        }
    };

    inline void report_syntax_error(const string_t &string) {
        ExceptionHandle::global_handle().add_exception(ExceptionType::SYNTAX_ERROR, string);
    }

    inline void report_semantic_error(const string_t &string) {
        ExceptionHandle::global_handle().add_exception(ExceptionType::SEMANTIC_ERROR, string);
    }
}
#endif //LISP_SIMULATOR_EXCEPTION_H
