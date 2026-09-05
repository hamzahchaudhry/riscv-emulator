#include <cstddef>
#include <cstdlib>
#include <print>
#include <span>
#include <string_view>

#include "debugger.h"
#include "hart.h"
#include "memory.h"
#include "types.h"

int main(int argc, char** argv) {
  std::span<char*> args{argv, static_cast<std::size_t>(argc)};

  if (argc < 2 || argc > 3) {
    std::println(stderr, "usage: riscv <test.bin> [--debug]");
    return EXIT_FAILURE;
  }

  const bool debug = args.size() == 3 && std::string_view{args.at(2)} == "--debug";

  if (args.size() == 3 && !debug) {
    std::println(stderr, "unknown option: {}", args.at(2));
    return EXIT_FAILURE;
  }

  constexpr rv32i_emu::u32 kMemoryBase = 0x80000000;
  constexpr auto kMemorySize = static_cast<std::size_t>(64 * 1024 * 1024);

  rv32i_emu::Memory memory(kMemoryBase, kMemorySize);

  const auto load_result = memory.LoadBinary(args.at(1), kMemoryBase);

  if (!load_result) {
    std::println(stderr, "failed to load file: {}", args.at(1));
    return EXIT_FAILURE;
  }

  rv32i_emu::Hart hart(kMemoryBase);

  if (debug) {
    rv32i_emu::Debugger debugger(hart, memory);
    debugger.Run();
    return EXIT_SUCCESS;
  }

  constexpr int kMaxCycles = 100'000;
  for (int i = 0; i < kMaxCycles; ++i) {
    const auto result = hart.Step(memory);

    if (result) continue;

    if (result.error() == rv32i_emu::Hart::Trap::kEnvironmentCall) {
      // riscv-tests
      const rv32i_emu::u32 status = hart.registers().ReadRegister(3);

      if (status == 1) {
        std::println("PASS");
        return EXIT_SUCCESS;
      }

      std::println("FAILED TEST: {}", status >> 1);
      return EXIT_FAILURE;
    }

    std::println("unexpected trap");
    return EXIT_FAILURE;
  }

  std::println("max cycles hit");
  return EXIT_FAILURE;
}
