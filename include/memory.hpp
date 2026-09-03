#pragma once

#include "types.hpp"

#include <cstddef>
#include <string>
#include <vector>

class Memory {
public:
  Memory(const std::string &file);

  u8 read_byte(u32 addr) const;
  u16 read_half(u32 addr) const;
  u32 read_word(u32 addr) const;
  void write_byte(u32 addr, u8 byte);
  void write_half(u32 addr, u16 half);
  void write_word(u32 addr, u32 word);

  std::size_t size() { return data_.size(); }

private:
  std::vector<u8> data_;
};
