#ifndef RV32I_EMU_DISASSEMBLER_H_
#define RV32I_EMU_DISASSEMBLER_H_

#include <string>

#include "instruction.h"

namespace rv32i_emu {

std::string Disassemble(const Instruction& instr);

}  // namespace rv32i_emu

#endif  // RV32I_EMU_DISASSEMBLER_H_
