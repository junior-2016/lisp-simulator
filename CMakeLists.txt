cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(lisp_simulator)
set(CMAKE_CXX_STANDARD 17)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Wconversion -pedantic -Werror=return-type")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GCC_COVERAGE_COMPILE_FLAGS}")

file(GLOB source_files "src/include/*.h" "src/include/*.cpp")

# find boost library
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.65.1)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(lisp_simulator src/main.cpp ${source_files} src/include/token.h src/include/config.h src/include/exception.h src/include/compile.h src/include/parser.cpp src/include/exception.cpp src/include/util.cpp src/include/compile.cpp src/include/evaluate.cpp src/include/scanner.cpp src/include/token.cpp)
    target_link_libraries(lisp_simulator ${Boost_LIBRARIES})
endif()
