#include "debug.hpp"
#include "decoder.hpp"
#include "disasm.hpp"

#include <print>

std::expected<void, Hart::Trap> trace_step(Hart &hart, Memory &memory) {
  const auto old_pc = hart.pc();
  const auto raw = memory.read_word(old_pc);
  const auto instr = decode(raw);
  const RegisterFile before = hart.registers();

  const auto result = hart.step(memory);

  if (!result) {
    if (instr)
      std::println("pc={:#04x} {} TRAP", old_pc, disasm(*instr));
    else
      std::println("pc={:#04x} instr={:#08x} illegal instruction", old_pc, raw);

    return result;
  }

  std::println("pc={:#04x} {} next={:#04x}", old_pc, disasm(*instr), hart.pc());

  for (u8 i = 1; i < 32; ++i) {
    const auto after = hart.registers().read(i);

    if (after != before.read(i))
      std::println("x{}={}", i, after);
  }

  std::println();
  return result;
}
