cmake_minimum_required(VERSION 3.19)
project(Ex1 C)
set(CMAKE_C_STANDARD 99)
link_directories(.)

add_executable(Ex1 main.c dry.c map.h map.c map_example_test.c chessTournament.c chessSystem.c chessTournament.h)
add_executable(chess chessSystem.c chessTournament.c chessSystem.h)

target_link_libraries(Ex1 libmap.a)

