cmake_minimum_required(VERSION 3.23)
project(sp5 C)

set(CMAKE_C_STANDARD 23)

add_executable(sp5 kar.c chat.c "with comment.c" subproc.c)
