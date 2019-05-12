cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(lisp_simulator)
set(CMAKE_CXX_STANDARD 17)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Wconversion -pedantic -Werror=return-type")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_COVERAGE_COMPILE_FLAGS}")

# 注意下面的 CMAKE_BUILD_TYPE 我更倾向于理解为一个变量,也就是说,通过这个变量设置,来影响CmakeLists中与这个变量有关的判断.
# 但是本身 CMAKE_BUILD_TYPE 跟编译时的 cmake -DCMAKE_BUILD_TYPE = ".." 完全不是同一个东西,
# 要真正控制编译模式为 Debug/Release 还是得通过 cmake -DCMAKE_BUILD_TYPE (这个可以通过clion编辑器设置).
# 甚至可以考虑将下面的 CMAKE_BUILD_TYPE 变量改成另外的变量名,反正它只是一个变量而已.
set(CMAKE_BUILD_TYPE "Release")
# 如果CMAKE_BUILD_TYPE变量等于字符串"Debug",就添加 DEBUG 的宏定义,这样代码里的 ifdef DEBUG ... 才有效
IF (CMAKE_BUILD_TYPE STREQUAL Debug) # STREQUAL 是字符串比较函数
    ADD_DEFINITIONS(-DDEBUG) # 添加宏定义用 -D<macro>
ENDIF ()

file(GLOB source_files "src/include/*.h" "src/include/*.cpp")

# find boost library
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.65.1)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(lisp_simulator src/main.cpp ${source_files} src/include/token.h src/include/config.h src/include/exception.h src/include/compile.h src/include/parser.cpp src/include/exception.cpp src/include/util.cpp src/include/compile.cpp src/include/evaluate.cpp src/include/scanner.cpp src/include/token.cpp)
    target_link_libraries(lisp_simulator ${Boost_LIBRARIES})
endif ()
