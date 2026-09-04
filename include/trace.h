#ifndef RV32I_EMU_TRACE_H_
#define RV32I_EMU_TRACE_H_

#include <expected>

#include "hart.h"
#include "memory.h"

namespace rv32i_emu {

std::expected<void, Hart::Trap> TraceStep(Hart& hart, Memory& memory);

}  // namespace rv32i_emu

#endif  // RV32I_EMU_TRACE_H_
