#ifndef RV32I_EMU_DEBUGGER_H_
#define RV32I_EMU_DEBUGGER_H_

#include <string_view>

#include "hart.h"
#include "memory.h"

namespace rv32i_emu {

class Debugger {
 public:
  explicit Debugger(Hart& hart, Memory& memory) : hart_(hart), memory_(memory) {};

 void Run();

 private:
  std::expected<void, Hart::Trap> Step();
  void Continue();
  void PrintRegisters() const;
  void PrintInstruction(u32 pc, std::string_view label) const;
  void PrintCurrentInstruction() const;

  Hart& hart_;
  Memory& memory_;
};

}  // namespace rv32i_emu

#endif  // RV32I_EMU_DEBUGGER_H_
