#pragma once

#include "types.hpp"

#include <string>
#include <vector>

class Memory {
public:
  Memory(const std::string &file, u32 base = 0);

  u8 read_byte(u32 addr) const;
  u16 read_half(u32 addr) const;
  u32 read_word(u32 addr) const;

  void write_byte(u32 addr, u8 byte);
  void write_half(u32 addr, u16 half);
  void write_word(u32 addr, u32 word);

private:
  std::size_t index(u32 address) const;

  u32 base_;
  std::vector<u8> data_;
};
