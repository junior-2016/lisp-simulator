//
// Created by junior on 19-5-9.
//

#ifndef LISP_SIMULATOR_IO_H
#define LISP_SIMULATOR_IO_H

#include "lisp.h"
#include "util.h"

namespace lisp {
    template<typename ... Args>
    void output(FILE *file, const char *fmt, Args &&... args) noexcept {
        if (file == STDERR) {
            std::fprintf(file, fmt, std::forward<Args>(args)...);
            std::this_thread::sleep_for(DELAY_FOR_ERROR_OUTPUT);
            // 因为错误输出和程序运行存在延时,所以这里将程序运行阻塞.
        } else {
            std::fprintf(file, fmt, std::forward<Args>(args)...);
        }
    }

    template<typename ... Args>
    void error_output(const char *fmt, Args &&... args) noexcept {
        output(STDERR, fmt, args...);
    }

    template<typename ... Args>
    void standard_output(const char *fmt, Args &&... args) noexcept {
        output(STDOUT, fmt, args...);
    }

}
#endif //LISP_SIMULATOR_IO_H
