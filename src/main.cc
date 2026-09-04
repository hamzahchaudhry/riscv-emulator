#include <cstdlib>
#include <print>
#include <span>
#include <string_view>
#include <utility>

#include "hart.h"
#include "memory.h"
#include "trace.h"
#include "types.h"

int main(int argc, char** argv) {
  std::span<char*> args{argv, static_cast<std::size_t>(argc)};

  if (argc < 2 || argc > 3) {
    std::println(stderr, "usage: riscv <test.bin> [--trace]");
    return EXIT_FAILURE;
  }

  const bool trace =
      args.size() == 3 && std::string_view{args.at(2)} == "--trace";

  if (args.size() == 3 && !trace) {
    std::println(stderr, "unknown option: {}", args.at(2));
    return EXIT_FAILURE;
  }

  constexpr rv32i_emu::u32 memory_base = 0x80000000;

  rv32i_emu::Hart hart(memory_base);
  auto memory_result = rv32i_emu::Memory::LoadFile(args.at(1), memory_base);

  if (!memory_result) {
    std::println("failed to open file: {}", args.at(1));
    return EXIT_FAILURE;
  }

  rv32i_emu::Memory memory = std::move(*memory_result);

  constexpr int kMaxCycles = 100'000;
  for (int i = 0; i < kMaxCycles; ++i) {
    const auto result = trace ? TraceStep(hart, memory) : hart.Step(memory);

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
