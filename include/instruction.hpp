#pragma once

#include "types.hpp"

#include <variant>

enum class Opcode : u8 {
  R = 0b0110011,
  I = 0b0010011,
  I_LOAD = 0b0000011,
  S = 0b0100011,
  B = 0b1100011,
  J_JAL = 0b1101111,
  I_JALR = 0b1100111,
  U_LUI = 0b0110111,
  U_AUIPC = 0b0010111,
  I_ENV = 0b1110011
};

enum class R_Funct3 : u8 {
  ADD_SUB = 0x0,
  XOR = 0x4,
  OR = 0x6,
  AND = 0x7,
  SLL = 0x1,
  SRL_SRA = 0x5,
  SLT = 0x2,
  SLTU = 0x3
};

enum class R_Funct7 : u8 {
  ADD = 0x00,
  SUB = 0x20,
  XOR = 0x00,
  OR = 0x00,
  AND = 0x00,
  SLL = 0x00,
  SRL = 0x00,
  SRA = 0x20,
  SLT = 0x00,
  SLTU = 0x00
};

enum class I_Funct3 : u8 {
  ADDI = 0x0,
  SLLI = 0x1,
  SLTI = 0x2,
  SLTIU = 0x3,
  XORI = 0x4,
  SRLI_SRAI = 0x5,
  ORI = 0x6,
  ANDI = 0x7
};

enum class I_LOAD_Funct3 : u8 {
  LB = 0x0,
  LH = 0x1,
  LW = 0x2,
  LBU = 0x4,
  LHU = 0x5
};

enum class I_Funct7 : u8 {
  SLLI = 0x00,
  SRLI = 0x00,
  SRAI = 0x20
};

enum class I_ENV_Funct3 : u8 {
  ECALL = 0x0,
  EBREAK = 0x0
};

enum class B_Funct3 : u8 {
  BEQ = 0x0,
  BNE = 0x1,
  BLT = 0x4,
  BGE = 0x5,
  BLTU = 0x6,
  BGEU = 0x7
};

enum class S_Funct3 : u8 {
  SB = 0x0,
  SH = 0x1,
  SW = 0x2
};

struct RType {
  Opcode opcode;
  u8 rd;
  u8 rs1;
  R_Funct3 funct3;
  u8 rs2;
  R_Funct7 funct7;
};

struct IType {
  Opcode opcode;
  u8 rd;
  std::variant<I_Funct3, I_LOAD_Funct3, I_ENV_Funct3> funct3;
  u8 rs1;
  i32 imm;
};

struct SType {
  Opcode opcode;
  S_Funct3 funct3;
  u8 rs1;
  u8 rs2;
  i32 imm;
};

struct BType {
  Opcode opcode;
  B_Funct3 funct3;
  u8 rs1;
  u8 rs2;
  i32 imm;
};

struct UType {
  Opcode opcode;
  u8 rd;
  u32 imm;
};

struct JType {
  Opcode opcode;
  u8 rd;
  i32 imm;
};

using Instruction = std::variant<RType, IType, SType, BType, UType, JType>;
