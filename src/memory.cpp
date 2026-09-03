#include "memory.hpp"

#include <fstream>
#include <stdexcept>

Memory::Memory(const std::string &file, u32 base)
    : base_{base} {
  std::ifstream mem(file, std::ios::binary);

  if (!mem)
    throw std::runtime_error("failed to open memory file");

  char byte;
  while (mem.get(byte))
    data_.push_back(static_cast<u8>(byte));
}

std::size_t Memory::index(u32 address) const {
  if (address < base_)
    throw std::out_of_range("address below memory base");

  return static_cast<std::size_t>(address - base_);
}

u8 Memory::read_byte(u32 address) const {
  return data_.at(index(address));
}

u16 Memory::read_half(u32 address) const {
  const auto i = index(address);

  return static_cast<u16>(data_.at(i)) |
         static_cast<u16>(static_cast<u16>(data_.at(i + 1)) << 8);
}

u32 Memory::read_word(u32 address) const {
  const auto i = index(address);

  return static_cast<u32>(data_.at(i)) |
         (static_cast<u32>(data_.at(i + 1)) << 8) |
         (static_cast<u32>(data_.at(i + 2)) << 16) |
         (static_cast<u32>(data_.at(i + 3)) << 24);
}

void Memory::write_byte(u32 addr, u8 byte) {
  data_[addr] = byte;
}

void Memory::write_half(u32 addr, u16 half) {
  data_[addr] = static_cast<u8>(half);
  data_[addr + 1] = static_cast<u8>(half >> 8);
}

void Memory::write_word(u32 addr, u32 word) {
  data_[addr] = static_cast<u8>(word);
  data_[addr + 1] = static_cast<u8>(word >> 8);
  data_[addr + 2] = static_cast<u8>(word >> 16);
  data_[addr + 3] = static_cast<u8>(word >> 24);
}
