//
// Created by junior on 19-5-9.
//
#include "lisp.h"
#include "io_tool.h"
#include "util.h"
#include "exception.h"
#include "scanner.h"
#include "parser.h"
#include "evaluate.h"
#include "compile.h"

namespace lisp {
    void man_help() {
        standard_output("%s\n", "---------------- MAN DOC HELP ------------------");

    }

    void interpreter_mode() {
        standard_output("%s\n", "------------------- Lisp simulator -------------------");
        standard_output("%s\n", "type QUIT to quit simulator; type HELP to gain man doc.");
        string_t input;
        string_t source;
        size_t i = 0;
        size_t line_number = 0;
        std::vector<char_t> check_source_paren; // 检查括号
        while (true) {
            if (check_source_paren.empty()) {
                standard_output("echo [%zu] > ", ++line_number);
            }
            std::getline(std::cin, input);
            if (input == "QUIT") break;
            else if (input == "HELP") man_help();
            else {
                for (i = 0; i < input.size(); i++) {
                    if (input[i] == '(') check_source_paren.push_back(input[i]);
                    if (input[i] == ')') {
                        if (!check_source_paren.empty()) {
                            check_source_paren.pop_back();
                            if (check_source_paren.empty()) {
                                i++;  // 保留最后一个 ')'
                                break;// 这里只提取圆括号合法的首行. 如果输入(define x 1)(define y 1)只提取(define x 1)
                            }
                        } else break;
                    }
                }
                source += input.substr(0, i);
                if (check_source_paren.empty()) {
                    if (source.empty()) {
                        error_output("%s\n", "The valid source input is empty.");
                    } else {
                        auto ast = parse(source);
                        if (ExceptionHandle::global_handle().hasException()) {
                            error_output("%s\n", ExceptionHandle::global_handle().to_string());
                        } else {
                            // evaluate

                        }
                    }
                    source = "";
                }
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