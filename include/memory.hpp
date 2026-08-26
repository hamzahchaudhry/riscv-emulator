#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Memory {
public:
  Memory(const std::string &file);

  std::uint32_t read_addr(std::uint32_t addr) const;

private:
  std::vector<std::uint8_t> data;
};
