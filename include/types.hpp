#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

using byte_t = std::uint8_t;
using word_t = std::uint32_t;

enum InstrType { R, I, S, B, U, J };
enum Instr { ADD, SUB, XOR, OR, AND, SLL, SRL, SRA, SLT, SLTU };

struct InstrEncoding {
  Instr instr;
  InstrType type;
  byte_t opcode : 7;
  byte_t funct3 : 3;
  byte_t funct7 : 7;
};

constexpr InstrEncoding encodings[]{
        {ADD, R, 0x33, 0x0, 0x00}, {SUB, R, 0x33, 0x0, 0x20}, {XOR, R, 0x33, 0x4, 0x00}, {OR, R, 0x33, 0x6, 0x00},  {AND, R, 0x33, 0x7, 0x00},
        {SLL, R, 0x33, 0x1, 0x00}, {SRL, R, 0x33, 0x5, 0x00}, {SRA, R, 0x33, 0x5, 0x20}, {SLT, R, 0x33, 0x2, 0x00}, {SLTU, R, 0x33, 0x3, 0x00},
};
#endif
