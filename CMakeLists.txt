cmake_minimum_required(VERSION 3.6)
project(simple_c_interpreter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -finput-charset=utf-8 -fexec-charset=GBK")

set(SOURCE_FILES main.cpp symbol_table.cpp symbol_table.h)
add_executable(simple_c_interpreter ${SOURCE_FILES})