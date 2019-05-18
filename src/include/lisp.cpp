//
// Created by junior on 19-5-9.
//
#include "lisp.h"
#include "util.h"
#include "io_tool.h"
#include "exception.h"
#include "scanner.h"
#include "parser.h"
#include "evaluate.h"

namespace lisp {
    void man_help() {
        standard_output("%s\n", "---------------- MAN DOC HELP ------------------");
        standard_output("%s\n", "(define atom <expr>): define a const atom which value is from <expr>.");
        standard_output("%s\n", "(defvar atom <expr>): define a variable which value is from <expr>.");
        standard_output("%s\n", "(lambda (atom list) (lambda_body)): declare a lambda function.");
        standard_output("%s\n", "(cond (bool_expr value_expr) (bool_expr value_expr) ... ): condition function.");
        standard_output("%s\n", "(eq? arg_0 arg_1): check arg_0 and arg_1 equal.");
        standard_output("%s\n", "(set! atom <expr>): change value of a variable atom.");
        standard_output("%s\n", "(+ arg_0 arg_1): add function.");
        standard_output("%s\n", "(- arg_0 arg_1): minus function.");
        standard_output("%s\n", "(* arg_0 arg_1): times function.");
        standard_output("%s\n", "(/ arg_0 arg_1): over function.");
        standard_output("%s\n", "True: true literal.");
        standard_output("%s\n", "False: false literal.");
        standard_output("%s\n", "-------------------------------------------------");
    }

    void interpreter_mode() {
        standard_output("%s\n", "------------------- Lisp simulator -------------------");
        standard_output("%s\n", "type QUIT to quit simulator; type HELP to gain man doc.");
        string_t input;
        string_t source;
        size_t i = 0;
        size_t line_number = 0;
        std::vector<char_t> check_source_paren; // 检查括号
        auto global_env = Env::global_env();
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
                        auto root = parse(source);
#ifdef DEBUG
                        auto ast_str = Ast::to_string(root);
                        standard_output("%s", ast_str.c_str());
#endif
                        if (ExceptionHandle::global_handle().hasException()) {
                            auto message = ExceptionHandle::global_handle().to_string();
                            error_output("%s", message.c_str());
                        } else {
                            // evaluate
                            auto value = eval(root, global_env);
                            standard_output("Value = %s\n", value_to_string(value).c_str());
                            if (ExceptionHandle::global_handle().hasException()) {
                                auto message = ExceptionHandle::global_handle().to_string();
                                error_output("%s", message.c_str());
                            }
                        }
                    }
                    source = "";
                    ExceptionHandle::global_handle().clearException(); // 最后一步很重要...
                }
            }
        }
    }

    /**
     * 存在 DEBUG 宏定义时才进行测试
     */
    void test_all() {
        UtilTestCase::test();
    }

    void main() {
#ifdef DEBUG
        test_all();
#endif
        interpreter_mode();
    }
}