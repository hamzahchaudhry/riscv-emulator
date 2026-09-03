cmake_minimum_required(VERSION 3.23)

project(riscv LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 26)

add_executable(riscv
  src/main.cpp
  src/hart.cpp
  src/decoder.cpp
  src/disasm.cpp
  src/memory.cpp
  src/register_file.cpp
  src/debug.cpp
)

target_include_directories(riscv
  PRIVATE
    include
)

target_compile_options(riscv PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wsign-conversion
)
