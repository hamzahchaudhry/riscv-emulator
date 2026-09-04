#ifndef HART_HPP
#define HART_HPP

#include "instruction.hpp"
#include "memory.hpp"
#include "register_file.hpp"
#include <expected>

class Hart {
 public:
  Hart(u32 reset_pc = 0) : pc_(reset_pc) {}

  enum class Trap {
    IllegalInstruction,
    EnvironmentCall,
    EnvironmentBreak
  };

  std::expected<void, Hart::Trap> step(Memory &memory);
  const RegisterFile &registers() const { return registers_; }
  u32 pc() const { return pc_; }

 private:
  u32 pc_ = 0;
  RegisterFile registers_;

  constexpr u32 add_offset(u32 base, i32 offset) { return base + static_cast<u32>(offset); }

  std::expected<u32, Trap> execute(const Instruction &instr, Memory &memory);

  std::expected<void, Trap> execute_register(const RegisterInstruction &instr);
  std::expected<void, Trap> execute_immediate(const ImmediateInstruction &instr);
  std::expected<void, Trap> execute_shift_immediate(const ShiftImmediateInstruction &instr);
  std::expected<void, Trap> execute_load(const LoadInstruction &instr, Memory &memory);
  std::expected<void, Trap> execute_store(const StoreInstruction &instr, Memory &memory);
  std::expected<u32, Trap> execute_branch(const BranchInstruction &instr);
  std::expected<u32, Trap> execute_jal(const Jal &instr);
  std::expected<u32, Trap> execute_jalr(const Jalr &instr);
  std::expected<void, Trap> execute_upper(const UpperInstruction &instr);
  std::expected<void, Trap> execute_system(const SystemInstruction &instr);
};

#endif  // HART_HPP
