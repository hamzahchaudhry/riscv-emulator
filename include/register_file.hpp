#pragma once

#include "types.hpp"

#include <array>

class RegisterFile {
public:
  u32 read(u8 index) const;
  void write(u8 index, u32 value);

private:
  std::array<u32, 32> registers_{};
};
