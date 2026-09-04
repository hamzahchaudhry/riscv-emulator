#include "register_file.h"

#include <cstdlib>

namespace rv32i_emu {

u32 RegisterFile::ReadRegister(u8 index) const noexcept {
  if (index >= registers_.size()) std::abort();
  if (index == 0) return 0;

  return registers_[index];  // NOLINT
}

void RegisterFile::WriteRegister(u8 index, u32 value) noexcept {
  if (index >= registers_.size()) std::abort();
  if (index == 0) return;

  registers_[index] = value;  // NOLINT
}

}  // namespace rv32i_emu
