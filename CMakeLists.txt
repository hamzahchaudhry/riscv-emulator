cmake_minimum_required(VERSION 3.23)

project(rv32i_emu LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(rv32i_emu
  src/main.cc
  src/hart.cc
  src/decoder.cc
  src/disassembler.cc
  src/memory.cc
  src/register_file.cc
  src/trace.cc
)

target_include_directories(rv32i_emu
  PRIVATE
    include
)

target_compile_options(rv32i_emu PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wsign-conversion
)

include(CTest)
add_subdirectory(tests)
