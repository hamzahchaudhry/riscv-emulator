#include "memory.h"

#include <expected>
#include <fstream>

namespace rv32i_emu {

std::expected<Memory, Memory::Error> Memory::LoadFile(const std::string& file,
                                                      u32 base) {
  std::ifstream input(file, std::ios::binary);

  if (!input) {
    return std::unexpected(Error::kFileOpenFailed);
  }

  Memory memory(base);
  char byte = 0;

  while (input.get(byte)) {
    memory.data_.push_back(static_cast<u8>(byte));
  }

  return memory;
}

std::expected<std::size_t, Memory::Error> Memory::Index(u32 address,
                                                        u8 size) const {
  if (address < base_) return std::unexpected(Error::kAddressOutOfRange);

  const auto index = static_cast<std::size_t>(address - base_);

  if (index > data_.size() || size > data_.size() - index) {
    return std::unexpected(Error::kAddressOutOfRange);
  }

  return index;
}

std::expected<u8, Memory::Error> Memory::ReadByte(u32 address) const {
  const auto index = Index(address, 1);

  if (!index) {
    return std::unexpected(index.error());
  }

  return data_.at(*index);
}

std::expected<u16, Memory::Error> Memory::ReadHalf(u32 address) const {
  const auto index = Index(address, 2);

  if (!index) {
    return std::unexpected(index.error());
  }

  return static_cast<u16>(data_.at(*index)) |
         static_cast<u16>(static_cast<u16>(data_.at(*index + 1)) << 8);
}

std::expected<u32, Memory::Error> Memory::ReadWord(u32 address) const {
  const auto index = Index(address, 4);

  if (!index) {
    return std::unexpected(index.error());
  }

  return static_cast<u32>(data_.at(*index)) |
         (static_cast<u32>(data_.at(*index + 1)) << 8) |
         (static_cast<u32>(data_.at(*index + 2)) << 16) |
         (static_cast<u32>(data_.at(*index + 3)) << 24);
}

[[nodiscard]] std::expected<void, Memory::Error> Memory::WriteByte(u32 address,
                                                                   u8 byte) {
  const auto index = Index(address, 1);

  if (!index) {
    return std::unexpected(index.error());
  }

  data_.at(*index) = byte;
  return {};
}

[[nodiscard]] std::expected<void, Memory::Error> Memory::WriteHalf(u32 address,
                                                                   u16 half) {
  const auto index = Index(address, 2);

  if (!index) {
    return std::unexpected(index.error());
  }

  data_.at(*index) = static_cast<u8>(half);
  data_.at(*index + 1) = static_cast<u8>(half >> 8);
  return {};
}

[[nodiscard]] std::expected<void, Memory::Error> Memory::WriteWord(u32 address,
                                                                   u32 word) {
  const auto index = Index(address, 4);

  if (!index) {
    return std::unexpected(index.error());
  }

  data_.at(*index) = static_cast<u8>(word);
  data_.at(*index + 1) = static_cast<u8>(word >> 8);
  data_.at(*index + 2) = static_cast<u8>(word >> 16);
  data_.at(*index + 3) = static_cast<u8>(word >> 24);
  return {};
}

}  // namespace rv32i_emu
