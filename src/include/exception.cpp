//
// Created by junior on 19-5-11.
//
#include "exception.h"

namespace lisp {
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
}