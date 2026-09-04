cmake_minimum_required(VERSION 3.23)

project(riscv LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 26)

add_executable(riscv
  src/main.cc
  src/hart.cc
  src/decoder.cc
  src/disassembler.cc
  src/memory.cc
  src/register_file.cc
  src/trace.cc
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

include(CTest)
add_subdirectory(tests)
