//
// Created by junior on 19-5-10.
//

#ifndef LISP_SIMULATOR_SYMBOL_TABLE_H
#define LISP_SIMULATOR_SYMBOL_TABLE_H

#include "lisp.h"
#include "util.h"

namespace lisp {
    /**
     * 符号表预定义的Symbol(Atom)(注意这些Symbol已经预定义,所以不允许在代码中重新定义)
     * defined_symbol_map
     * [Atom(原子名称)  Type(类型)  Syntax(语义)]
     * [  True          Bool      true(C++基本类型)       ]
     * [  False         Bool      false                  ]
     * [   +            Fun       [](a,b)->{return a+b;} ]
     * [   -            Fun                              ]
     * [   *            Fun                              ]
     * [   /            Fun                              ]
     * [   %            Fun                              ]
     * 其他的还有: > < >= <= == !=
     */
    class symbol_table {

    public:
        symbol_table &global_table() {

        }
        symbol_table &operator=(const symbol_table &) = delete;
        symbol_table(const symbol_table &) = delete;
    private:
        static symbol_table table;
        symbol_table() = default;

    };
}

#endif //LISP_SIMULATOR_SYMBOL_TABLE_H