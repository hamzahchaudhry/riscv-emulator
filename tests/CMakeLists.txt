find_program(
  RISCV_GCC
  riscv32-unknown-elf-gcc
  REQUIRED
)

find_program(
  RISCV_OBJCOPY
  riscv32-unknown-elf-objcopy
  REQUIRED
)

file(
  GLOB test_sources
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.S"
)

set(test_binaries)

foreach(source IN LISTS test_sources)
  get_filename_component(name "${source}" NAME_WE)

  set(elf "${CMAKE_CURRENT_BINARY_DIR}/${name}.elf")
  set(bin "${CMAKE_CURRENT_BINARY_DIR}/${name}.bin")

  add_custom_command(
    OUTPUT "${bin}"
    BYPRODUCTS "${elf}"

    COMMAND "${RISCV_GCC}"
      -march=rv32i
      -mabi=ilp32
      -nostdlib
      -nostartfiles
      -I "${CMAKE_CURRENT_SOURCE_DIR}/env"
      -T "${CMAKE_CURRENT_SOURCE_DIR}/env/link.ld"
      "${source}"
      -o "${elf}"

    COMMAND "${RISCV_OBJCOPY}"
      -O binary
      "${elf}"
      "${bin}"

    DEPENDS
      "${source}"
      "${CMAKE_CURRENT_SOURCE_DIR}/env/riscv_test.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/env/test_macros.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/env/link.ld"

    COMMENT "Building RV32I test ${name}"
    VERBATIM
  )

  list(APPEND test_binaries "${bin}")

  add_test(
    NAME "rv32i.${name}"
    COMMAND riscv "${bin}"
  )
endforeach()

add_custom_target(
  rv32i_test_binaries ALL
  DEPENDS ${test_binaries}
)
