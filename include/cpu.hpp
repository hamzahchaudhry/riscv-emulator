#ifndef CPU_HPP
#define CPU_HPP

#include "memory.hpp"
#include "types.hpp"
#include <array>

class CPU {
public:
  void emulate_cycle(Memory &memory);
  word_t read_reg(byte_t index) const;

private:
  word_t PC = 0;
  std::array<word_t, 32> x{};

  bool write_reg(byte_t index, word_t val);
  DecodedInstr decode(word_t instr);
  void execute(word_t instr);
  word_t sign_extend(word_t instr);
  void execute_i_type(word_t instr);
  void execute_r_type(word_t instr);
};

#endif
