#pragma once

#include "hart.hpp"
#include "memory.hpp"

std::expected<void, Hart::Trap> trace_step(Hart &hart, Memory &memory);
