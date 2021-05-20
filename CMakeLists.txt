cmake_minimum_required(VERSION 3.19)
project(Ex1 C)
set(CMAKE_C_STANDARD 99)
link_directories(.)
add_executable(Dry dry.c)
add_executable(Map main.c  map.h map.c map_example_test.c)
add_executable(chess chessSystem.c chessTournament.c chessPlayer.h chessTournament.h chessSystem.h chessSystemTestsExample.c chessCopyFreeCompareFunctions.h chessCopyFreeCompareFunctions.c chessPlayer.c chessGame.c chessGame.h)

target_link_libraries(chess libmap.a)

