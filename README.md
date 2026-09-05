# riscv-emulator

A RISC-V emulator written in C++, currently supporting RV32I.

## Building

Requires CMake, a C++26 compiler, `riscv32-unknown-elf-gcc`, and
`riscv32-unknown-elf-objcopy`.

```sh
cmake -B build
cmake --build build -j
```

## Running

```sh
./build/rv32i_emu program.bin
```

Flat binaries are loaded at `0x80000000`.

Run with the interactive debugger:

```sh
./build/rv32i_emu program.bin --debug
```

The debugger supports stepping, continuing, breakpoints, register inspection,
restarting, and quitting.

## Testing

```sh
ctest --test-dir build --output-on-failure
```

The assembly tests come from
[`riscv-tests`](https://github.com/riscv-software-src/riscv-tests). I adapted
the test environment and linker script to build RV32I flat binaries, report
results through `ECALL` and `gp`, and run every test through CTest.

The emulator currently passes all 37 included RV32I instruction tests.
