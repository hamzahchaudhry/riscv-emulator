#include "memory.hpp"

#include <fstream>
#include <stdexcept>

Memory::Memory(const std::string &file) {
  std::ifstream mem(file, std::ios::binary);

  if (!mem)
    throw std::runtime_error("failed to open memory file");

  char byte;
  while (mem.get(byte))
    data.push_back(static_cast<byte_t>(byte));
}

word_t Memory::read_addr(word_t addr) const {
  return static_cast<word_t>(data.at(addr)) | (static_cast<word_t>(data.at(addr + 1)) << 8) | (static_cast<word_t>(data.at(addr + 2)) << 16) |
         (static_cast<word_t>(data.at(addr + 3)) << 24);
}
