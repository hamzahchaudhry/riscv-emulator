find_program(RISCV_GCC riscv32-unknown-elf-gcc REQUIRED)
find_program(RISCV_OBJCOPY riscv32-unknown-elf-objcopy REQUIRED)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.S")
file(GLOB TEST_ENV CONFIGURE_DEPENDS "env/*")

foreach(source IN LISTS TEST_SOURCES)
  get_filename_component(name "${source}" NAME_WE)
  set(output "${CMAKE_CURRENT_BINARY_DIR}/${name}")

  add_custom_command(
    OUTPUT "${output}.bin"
    BYPRODUCTS "${output}.elf"
    COMMAND "${RISCV_GCC}" -march=rv32i -mabi=ilp32 -nostdlib
            -I "${CMAKE_CURRENT_SOURCE_DIR}/env"
            -T "${CMAKE_CURRENT_SOURCE_DIR}/env/link.ld"
            "${source}" -o "${output}.elf"
    COMMAND "${RISCV_OBJCOPY}" -O binary "${output}.elf" "${output}.bin"
    DEPENDS "${source}" ${TEST_ENV}
    COMMENT "Building RV32I test ${name}"
    VERBATIM
  )

  list(APPEND TEST_BINARIES "${output}.bin")
  add_test(NAME "rv32i.${name}" COMMAND rv32i_emu "${output}.bin")
endforeach()

add_custom_target(rv32i_test_binaries ALL DEPENDS ${TEST_BINARIES})
