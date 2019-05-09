//
// Created by junior on 19-5-8.
//

#ifndef LISP_SIMULATOR_LISP_H
#define LISP_SIMULATOR_LISP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#include <cstdint>
#include <cstdio>
#include <cstddef>

/**
 * (define ID value):
 * 不能重绑定, 内部已经默认绑定了 +,-,*,/ 四个标识符, 所以如果写 (define + 5) 就会报variable + unbound错误.
 * 又比如: 已经写 (define add 5), 如果再写 (define add 4) 就会报 variable add unbound 错误.
 * 并且注意到 lisp 的 variable 是不允许 unbound 的,即每一个 ID 只能绑定一个数值/function,并且绑定后不能再修改.
 */
namespace lisp {
    void main();
}
#endif //LISP_SIMULATOR_LISP_H
