cmake_minimum_required(VERSION 3.15)

project(Chip8)

set(CXX_STANDARD 20)

find_package(Catch2 REQUIRED)
find_package(SDL2 REQUIRED)

add_library(Chip8Lib chip8.h chip8.cpp)

add_executable(Chip8App run.cpp)
target_link_libraries(Chip8App PRIVATE Chip8Lib SDL2::SDL2)

add_executable(Chip8Assembler assembler.cpp assembler.h)
target_link_libraries(Chip8Assembler PRIVATE Chip8Lib)

add_executable(tests tests_main.cpp tests.cpp)
# We might want to remove the dependence on SDL2?
target_link_libraries(tests PRIVATE Chip8Lib Catch2::Catch2 SDL2::SDL2)
