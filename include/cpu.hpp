#pragma once

#include "instruction.hpp"
#include "memory.hpp"
#include <array>
#include <cstdint>
#include <optional>

class CPU {
public:
  CPU() {};
  void emulate_cycle(Memory &memory);
  std::uint32_t read_reg(std::uint8_t index) const;

private:
  std::uint32_t PC = 0;
  std::array<std::uint32_t, 32> x{};

  bool write_reg(std::uint8_t index, std::uint32_t val);
  std::int32_t sign_extend(std::uint32_t imm, std::uint32_t n);
  std::optional<Instruction> decode(const std::uint32_t &instr);
  void execute(const Instruction &instr);
};
