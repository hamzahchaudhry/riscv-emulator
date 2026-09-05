#include "debugger.h"

#include <charconv>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <string_view>

#include "decoder.h"
#include "disassembler.h"
#include "register_file.h"

namespace rv32i_emu {

Debugger::ExitReason Debugger::Run() {
  std::string previous_command;

  PrintCurrentInstruction();

  while (true) {
    std::print("(rv32i_emu) ");

    std::string line;
    if (!std::getline(std::cin, line)) {
      std::println();
      return ExitReason::kQuit;
    }

    if (line.empty()) line = previous_command;

    std::istringstream input(line);
    std::string command;
    input >> command;

    if (command.empty()) continue;

    if (command == "b" || command == "break") {
      std::string address_argument;
      std::string extra_argument;

      if (!(input >> address_argument) || input >> extra_argument) {
        std::println("usage: b[reak] <hex address>");
        continue;
      }

      AddBreakpoint(address_argument);
      previous_command.clear();
      continue;
    }

    if (command == "s" || command == "step") {
      previous_command = Step() ? "s" : "";
    } else if (command == "c" || command == "continue") {
      Continue();
      previous_command.clear();
    } else if (command == "regs") {
      PrintRegisters();
      previous_command = "regs";
    } else if (command == "r" || command == "restart") {
      return ExitReason::kRestart;
    } else if (command == "pc") {
      std::println("pc: {:#010x}", hart_.pc());
      previous_command = "pc";
    } else if (command == "q" || command == "quit")
      return ExitReason::kQuit;
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

    if (breakpoints_.contains(hart_.pc())) {
      std::println("breakpoint hit at {:#010x}", hart_.pc());
      PrintCurrentInstruction();
      return;
    }
  }
}

void Debugger::AddBreakpoint(std::string_view address_argument) {
  std::string_view address_text = address_argument;
  if (address_text.starts_with("0x") || address_text.starts_with("0X")) {
    address_text.remove_prefix(2);
  }

  u32 address = 0;
  const auto [end, error] =
      std::from_chars(address_text.data(), address_text.data() + address_text.size(), address, 16);

  if (error != std::errc{} || end != address_text.data() + address_text.size()) {
    std::println("invalid address: {}", address_argument);
    return;
  }

  if (!IsValidAddress(address)) {
    std::println("invalid breakpoint address: {:#010x}", address);
    return;
  }

  const auto [unused, inserted] = breakpoints_.insert(address);
  if (inserted)
    std::println("breakpoint set at {:#010x}", address);
  else
    std::println("breakpoint already exists at {:#010x}", address);
}

bool Debugger::IsValidAddress(u32 address) const {
  return address % 4 == 0 && memory_.ReadWord(address).has_value();
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
