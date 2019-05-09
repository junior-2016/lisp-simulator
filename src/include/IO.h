//
// Created by junior on 19-5-9.
//

#ifndef LISP_SIMULATOR_IO_H
#define LISP_SIMULATOR_IO_H

#include "lisp.h"

namespace lisp {
    /**
     * lisp.h
     * struct scanner_input{
     *    std::vector<std::string> input;
     * }
     * std::optional<scanner_output> read_single_line( iostream );
     * => 当没有遇到 EOF(对于文件流输入) 或者 Ctrl+Z(对于控制台输入),read_single_line始终可以返回一个source_line.
     * => 返回一个source_line不是基于回车键,而是基于 ( ... ) 的闭包.
     * 比如输入 ( define func  \n
     *           lambda ( x y z ) \n
     *           ( x + y + z ) )  ( if \n
     *           ...
     * 会提取出第一行 (define ..... ) (一直到 ( if 之前 ), 至于( if \n 会被丢弃...
     * 如果输入　( + 1 1) (- 1 1) 会处理多行数据
     *
     * void main_loop(){
     *     while( (out = read_single_line(file_or_command_io)).has_value() ){
     *          parse_tree = Parser::parse(out.value.source_line); // Parser 会调用 Scanner 处理 source_line
     *          .. analyser
     *          .. evaluate
     *          .. output to command or binary-file
     *
     *     }
     * }
     *
     */
}
#endif //LISP_SIMULATOR_IO_H
