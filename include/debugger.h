#ifndef RV32I_EMU_DEBUGGER_H_
#define RV32I_EMU_DEBUGGER_H_

#include <set>
#include <string_view>

#include "hart.h"
#include "memory.h"

namespace rv32i_emu {

class Debugger {
 public:
  enum class ExitReason : u8 {
    kQuit,
    kRestart,
  };

  explicit Debugger(Hart& hart, Memory& memory) : hart_(hart), memory_(memory) {};

  ExitReason Run();

 private:
  std::expected<void, Hart::Trap> Step();
  void Continue();
  void AddBreakpoint(std::string_view address_argument);
  [[nodiscard]] bool IsValidAddress(u32 address) const;
  void PrintRegisters() const;
  void PrintInstruction(u32 pc, std::string_view label) const;
  void PrintCurrentInstruction() const;

  Hart& hart_;
  Memory& memory_;

  std::set<u32> breakpoints_;
};

}  // namespace rv32i_emu

#endif  // RV32I_EMU_DEBUGGER_H_
