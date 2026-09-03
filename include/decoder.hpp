#pragma once

#include "instruction.hpp"

#include <optional>

i32 sign_extend(u32 imm, u32 n);
std::optional<Instruction> decode(u32 instr);
