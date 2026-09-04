#include "register_file.hpp"
#include <stdexcept>

u32 RegisterFile::read(u8 index) const {
  if (index >= registers_.size())
    throw std::out_of_range("invalid register index");
  return (index == 0) ? 0 : registers_[index];
}

void RegisterFile::write(u8 index, u32 val) {
  if (index > 0 && index < registers_.size())
    registers_.at(index) = val;
}
