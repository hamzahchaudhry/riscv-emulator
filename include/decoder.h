#ifndef RV32I_EMU_DECODER_H_
#define RV32I_EMU_DECODER_H_

#include <optional>

#include "instruction.h"
#include "types.h"

namespace rv32i_emu {

std::optional<Instruction> Decode(u32 raw);

}  // namespace rv32i_emu

#endif  // RV32I_EMU_DECODER_H_
