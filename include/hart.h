#ifndef RV32I_EMU_HART_H_
#define RV32I_EMU_HART_H_

#include <expected>

#include "instruction.h"
#include "memory.h"
#include "register_file.h"
#include "types.h"

namespace rv32i_emu {

class Hart {
 public:
  enum class Trap : u8 {
    kIllegalInstruction,
    kInstructionAccessFault,
    kLoadAccessFault,
    kStoreAccessFault,
    kEnvironmentCall,
    kEnvironmentBreak
  };

  explicit Hart(u32 reset_pc = 0) : pc_(reset_pc) {}

  std::expected<void, Hart::Trap> Step(Memory& memory);

  [[nodiscard]] u32 pc() const noexcept { return pc_; }
  [[nodiscard]] const RegisterFile& registers() const noexcept { return registers_; }

 private:
  std::expected<u32, Trap> Execute(const Instruction& instr, Memory& memory);
  std::expected<void, Trap> ExecuteRegister(const RegisterInstruction& instr);
  std::expected<void, Trap> ExecuteImmediate(const ImmediateInstruction& instr);
  std::expected<void, Trap> ExecuteShiftImmediate(const ShiftImmediateInstruction& instr);
  std::expected<void, Trap> ExecuteLoad(const LoadInstruction& instr, Memory& memory);
  std::expected<void, Trap> ExecuteStore(const StoreInstruction& instr, Memory& memory);
  std::expected<u32, Trap> ExecuteBranch(const BranchInstruction& instr);
  std::expected<u32, Trap> ExecuteJal(const Jal& instr);
  std::expected<u32, Trap> ExecuteJalr(const Jalr& instr);
  std::expected<void, Trap> ExecuteUpper(const UpperInstruction& instr);
  static std::expected<void, Trap> ExecuteSystem(const SystemInstruction& instr);
  static constexpr u32 AddOffset(u32 base, i32 offset) { return base + static_cast<u32>(offset); }

  u32 pc_ = 0;
  RegisterFile registers_;
};

}  // namespace rv32i_emu

#endif  // RV32I_EMU_HART_H_
