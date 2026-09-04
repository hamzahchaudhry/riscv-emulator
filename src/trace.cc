#include "trace.h"

#include <expected>
#include <print>
#include <string>

#include "decoder.h"
#include "disassembler.h"

namespace rv32i_emu {

std::expected<void, Hart::Trap> TraceStep(Hart& hart, Memory& memory) {
  const auto old_pc = hart.pc();
  const auto raw_result = memory.ReadWord(old_pc);

  if (!raw_result) {
    std::println("TRAP: pc=0x{:08x} instruction access fault", old_pc);
    return std::unexpected(Hart::Trap::kInstructionAccessFault);
  }

  const u32 raw = *raw_result;
  const auto instr = Decode(raw);

  const std::string instruction_text = instr ? Disassemble(*instr) : "<unknown>";
  const RegisterFile before = hart.registers();

  const auto result = hart.Step(memory);

  if (!result) {
    std::println("TRAP: pc=0x{:08x} raw=0x{:08x} {} next=0x{:08x}", old_pc, raw, instruction_text,
                 hart.pc());
    return result;
  }

  std::println("pc=0x{:08x} raw=0x{:08x} {} next=0x{:08x}", old_pc, raw, instruction_text,
               hart.pc());

  constexpr u8 kRegisterCount = 32;
  for (u8 i = 1; i < kRegisterCount; ++i) {
    const auto after = hart.registers().ReadRegister(i);

    if (after != before.ReadRegister(i))
      std::println("x{}: 0x{:08x} -> 0x{:08x}", i, before.ReadRegister(i), after);
  }

  std::println();
  return result;
}

}  // namespace rv32i_emu
