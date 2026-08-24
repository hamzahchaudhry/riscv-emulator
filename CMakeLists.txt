cmake_minimum_required(VERSION 3.23)

project(riscv LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

add_executable(riscv
    src/main.cpp
    src/cpu.cpp
    src/memory.cpp
)

target_include_directories(riscv
    PRIVATE
        include
)
