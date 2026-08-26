#pragma once

#include <cstdint>
#include <variant>

enum class Opcode : std::uint8_t {
  R = 0b0110011,
  I = 0b0010011,
  I_LOAD = 0b0000011,
  S = 0b0100011,
  B = 0b1100011,
  J_JAL = 0b1101111,
  I_JALR = 0b1100111,
  U_LUI = 0b0110111,
  U_AUIPC = 0b0010111
};

enum class R_Funct3 : std::uint8_t {
  ADD_SUB = 0x0,
  SLL = 0x1,
  SLT = 0x2,
  SLTU = 0x3,
  XOR = 0x4,
  SRL_SRA = 0x5,
  OR = 0x6,
  AND = 0x7
};

enum class R_Funct7 : std::uint8_t { ADD = 0x00, SUB = 0x20, SRL = 0x00, SRA = 0x20 };

enum class I_Funct3 : std::uint8_t {
  ADDI = 0x0,
  SLLI = 0x1,
  SLTI = 0x2,
  SLTIU = 0x3,
  XORI = 0x4,
  SRLI_SRAI = 0x5,
  ORI = 0x6,
  ANDI = 0x7
};

enum class I_Funct7 : std::uint8_t { SLLI = 0x00, SRLI = 0x00, SRAI = 0x20 };

struct RType {
  Opcode opcode;
  std::uint8_t rd;
  R_Funct3 funct3;
  std::uint8_t rs1;
  std::uint8_t rs2;
  R_Funct7 funct7;
};

struct IType {
  Opcode opcode;
  std::uint8_t rd;
  I_Funct3 funct3;
  std::uint8_t rs1;
  std::int32_t imm;
};

struct SType {
  Opcode opcode;
  std::uint8_t funct3;
  std::uint8_t rs1;
  std::uint8_t rs2;
  std::int32_t imm;
};

struct BType {
  Opcode opcode;
  std::uint8_t funct3;
  std::uint8_t rs1;
  std::uint8_t rs2;
  std::int32_t imm;
};

struct UType {
  Opcode opcode;
  std::uint8_t rd;
  std::int32_t imm;
};

struct JType {
  Opcode opcode;
  std::uint8_t rd;
  std::int32_t imm;
};

using Instruction = std::variant<RType, IType, SType, BType, UType, JType>;
