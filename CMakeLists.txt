cmake_minimum_required(VERSION 3.19)
project(Ex1 C)
link_directories(.)
set(CMAKE_C_STANDARD 99)

add_executable(Ex1 main.c dry.c map.h map.c test.c)
