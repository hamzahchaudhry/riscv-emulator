#include "debugger.h"

#include <iostream>
#include <print>
#include <string>

#include "decoder.h"
#include "disassembler.h"
#include "register_file.h"

namespace rv32i_emu {

void Debugger::Run() {
  std::string previous_command;

  PrintCurrentInstruction();

  while (true) {
    std::print("(rv32i_emu) ");

    std::string line;
    if (!std::getline(std::cin, line)) {
      std::println();
      return;
    }

    if (line.empty()) line = previous_command;

    if (line == "s" || line == "step") {
      previous_command = Step() ? "s" : "";
    } else if (line == "c" || line == "continue") {
      Continue();
      previous_command.clear();
    } else if (line == "r" || line == "regs") {
      PrintRegisters();
      previous_command = "r";
    } else if (line == "pc") {
      std::println("pc: {:#010x}", hart_.pc());
      previous_command = "pc";
    } else if (line == "q" || line == "quit")
      return;
    else
      std::println("huh?");

    std::println();
  }
}

std::expected<void, Hart::Trap> Debugger::Step() {
  const u32 old_pc = hart_.pc();
  const RegisterFile before = hart_.registers();
  const auto result = hart_.Step(memory_);

  if (!result) {
    PrintInstruction(old_pc, "trapped");
    return result;
  }

  PrintInstruction(old_pc, "executed");

  constexpr u8 kRegisterCount = 32;
  for (u8 index = 1; index < kRegisterCount; ++index) {
    const u32 old_value = before.ReadRegister(index);
    const u32 new_value = hart_.registers().ReadRegister(index);

    if (old_value != new_value) {
      std::println("x{}: {:#010x} -> {:#010x}", index, old_value, new_value);
    }
  }

  PrintCurrentInstruction();
  return result;
}

void Debugger::Continue() {
  while (true) {
    const u32 old_pc = hart_.pc();
    const auto result = hart_.Step(memory_);

    if (!result) {
      PrintInstruction(old_pc, "trapped");
      return;
    }
  }
}

void Debugger::PrintRegisters() const {
  for (u8 row = 0; row < 8; ++row) {
    for (u8 column = 0; column < 4; ++column) {
      const u8 index = static_cast<u8>(row + (column * 8));
      std::print("x{:<2}: {:#010x}{}", index, hart_.registers().ReadRegister(index),
                 column == 3 ? "\n" : "  ");
    }
  }
}

void Debugger::PrintInstruction(u32 pc, std::string_view label) const {
  const auto raw = memory_.ReadWord(pc);

  if (!raw) {
    std::println("{:<8} {:#010x}: <instruction access fault>", label, pc);
    return;
  }

  const auto instruction = Decode(*raw);

  if (!instruction) {
    std::println("{:<8} {:#010x}: <unknown> ({:#010x})", label, pc, *raw);
    return;
  }

  std::println("{:<8} {:#010x}: {}", label, pc, Disassemble(*instruction));
}

void Debugger::PrintCurrentInstruction() const { PrintInstruction(hart_.pc(), "next"); }

}  // namespace rv32i_emu
