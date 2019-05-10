//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_CONFIG_H
#define LISP_SIMULATOR_CONFIG_H

#include "lisp.h"

namespace lisp {

#define USE_DOUBLE_AS_NUMBER

#define DELAY_FOR_ERROR_OUTPUT 0.3s

    /* 配置控制台输出流和错误流,这样可以轻松将信息输出到某个日志文件里,而不需要改任何代码. */
    FILE *STDOUT = stdout;
    FILE *STDERR = stderr;

}
#endif //LISP_SIMULATOR_CONFIG_H
