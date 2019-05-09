//
// Created by junior on 19-5-9.
//
#include "lisp.h"
#include "util.h"
#include "io_tool.h"
#include "scanner.h"
#include "parser.h"

namespace lisp {
    void man_help() {
        printf("---------------- MAN DOC HELP ------------------\n");

    }

    void interpreter_mode() {
        string_t string;
        size_t line_number = 0;
        printf("------------------- Lisp simulator -------------------\n");
        printf("type QUIT to quit simulator; type HELP to gain man doc.\n");
        while (true) {
            printf("echo [%zu] > ", ++line_number);
            std::getline(std::cin, string);
            if (string == "QUIT") break;
            else if (string == "HELP") man_help();
            else {
                // 进入 scanner => parser 阶段.
            }
        }
    }

    void compile_mode(int argc, char **argv) {
        for (auto i = 1; i < argc; i++) {
            string_ptr file_name = make_ptr<string_t>(argv[i]);

        }
    }

    void main(int argc, char **argv) {
        if (argc <= 1) { // 没有多余的命令行参数,进入解释器模式
            interpreter_mode();
        } else { // 如果有其他命令行参数,进入编译模式,尝试将命令行参数解读为源文件并编译
            compile_mode(argc, argv);
        }
    }
}