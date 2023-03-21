cmake_minimum_required(VERSION 3.23)
project(sp5 C)

set(CMAKE_C_STANDARD 23)

add_executable(sp5 main_win.c chat.c "with comment.c" subproc_win.c main_linux.c subproc.c)
