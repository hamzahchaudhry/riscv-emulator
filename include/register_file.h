#ifndef RV32I_EMU_REGISTER_FILE_H_
#define RV32I_EMU_REGISTER_FILE_H_

#include <array>

#include "types.h"

namespace rv32i_emu {

class RegisterFile {
 public:
  void WriteRegister(u8 index, u32 value) noexcept;
  [[nodiscard]] u32 ReadRegister(u8 index) const noexcept;

 private:
  std::array<u32, 32> registers_{};
};

}  // namespace rv32i_emu

#endif  // RV32I_EMU_REGISTER_FILE_H_
