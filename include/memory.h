#ifndef RV32I_EMU_MEMORY_H_
#define RV32I_EMU_MEMORY_H_

#include <expected>
#include <string>
#include <vector>

#include "types.h"

namespace rv32i_emu {

class Memory {
 public:
  enum class Error : u8 {
    kFileOpenFailed,
    kAddressOutOfRange,
  };

  static std::expected<Memory, Error> LoadFile(const std::string& file,
                                               u32 base = 0);

  [[nodiscard]] std::expected<u8, Error> ReadByte(u32 address) const;
  [[nodiscard]] std::expected<u16, Error> ReadHalf(u32 address) const;
  [[nodiscard]] std::expected<u32, Error> ReadWord(u32 address) const;

  [[nodiscard]] std::expected<void, Error> WriteByte(u32 address, u8 byte);
  [[nodiscard]] std::expected<void, Error> WriteHalf(u32 address, u16 half);
  [[nodiscard]] std::expected<void, Error> WriteWord(u32 address, u32 word);

 private:
  explicit Memory(u32 base) : base_(base) {}

  [[nodiscard]] std::expected<std::size_t, Error> Index(u32 address,
                                                        u8 size) const;

  u32 base_;
  std::vector<u8> data_;
};

}  // namespace rv32i_emu

#endif  // RV32I_EMU_MEMORY_H_
