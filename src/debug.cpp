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
      std::println("TRAP: pc=0x{:08x} raw=0x{:08x} {} next=0x{:08x}", old_pc, raw, disasm(*instr), hart.pc());
    else
      std::println("pc=0x{:08x} raw=0x{:08x} {} next=0x{:08x}", old_pc, raw, disasm(*instr), hart.pc());

    return result;
  }

  std::println("pc=0x{:08x}, raw=0x{:08x}, {}, next=0x{:08x}", old_pc, raw, disasm(*instr), hart.pc());

  for (u8 i = 1; i < 32; ++i) {
    const auto after = hart.registers().read(i);

    if (after != before.read(i))
      std::println("x{}: 0x{:08x} -> 0x{:08x}", i, before.read(i), after);
  }

  std::println();
  return result;
}
