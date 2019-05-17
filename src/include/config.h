//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_CONFIG_H
#define LISP_SIMULATOR_CONFIG_H

namespace lisp {
#define USE_INT_AS_NUMBER
    // 默认使用整数环境,如果用浮点数可能作业里的一些test case会报错(第四个case).
    // (因为浮点数除法得到精确的浮点数,而整数除法会截取,所以在递归cond比较的时候可能浮点数会陷入死循环)

#define DELAY_FOR_ERROR_OUTPUT 0.3s

    /* 配置控制台输出流和错误流,这样可以轻松将信息输出到某个日志文件里,而不需要改任何代码. */
#define STDOUT stdout
#define STDERR stderr

}
#endif //LISP_SIMULATOR_CONFIG_H
