#pragma once

#include "instruction.hpp"
#include "memory.hpp"
#include "register_file.hpp"
#include <expected>

class Hart {
public:
  enum class Trap {
    IllegalInstruction
  };

  std::expected<void, Hart::Trap> step(Memory &memory);
  const RegisterFile &registers() const { return registers_; }
  u32 pc() const { return pc_; }

private:
  u32 pc_ = 0;
  RegisterFile registers_;

  constexpr u32 add_offset(u32 base, i32 offset) { return base + static_cast<u32>(offset); }

  std::expected<u32, Trap> execute(const Instruction &instr, Memory &memory);
  std::expected<void, Trap> execute_r_type(const RType &instr);
  std::expected<void, Trap> execute_i_type(const IType &instr);
  std::expected<u32, Trap> execute_b_type(const BType &instr);
  std::expected<u32, Trap> execute_j_type(const JType &instr);
  std::expected<void, Trap> execute_u_type(const UType &instr);
  std::expected<void, Trap> execute_s_type(const SType &instr, Memory &memory);
  std::expected<void, Trap> execute_load(const IType &instr, Memory &memory);
};
