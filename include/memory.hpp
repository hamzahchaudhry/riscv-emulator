#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "types.hpp"

#include <string>
#include <vector>

class Memory {
public:
  Memory(const std::string &file);

  word_t read_addr(word_t addr) const;

private:
  std::vector<byte_t> data;
};

#endif
