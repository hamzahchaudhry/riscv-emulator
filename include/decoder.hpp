#ifndef DECODER_HPP
#define DECODER_HPP

#include "instruction.hpp"

#include <optional>

std::optional<Instruction> decode(u32 instr);

std::optional<RegisterInstruction> decode_register(const u32 raw);
std::optional<ImmediateInstruction> decode_immediate(const u32 raw);
std::optional<ShiftImmediateInstruction> decode_shift_immediate(const u32 raw);
std::optional<LoadInstruction> decode_load(const u32 raw);
std::optional<StoreInstruction> decode_store(const u32 raw);
std::optional<BranchInstruction> decode_branch(const u32 raw);
std::optional<UpperInstruction> decode_upper(const u32 raw, UpperOp op);
std::optional<Jal> decode_jal(const u32 raw);
std::optional<Jalr> decode_jalr(const u32 raw);
std::optional<SystemInstruction> decode_system(const u32 raw);

namespace encoding {

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
  I_SYS = 0b1110011
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

enum class I_SYS_Funct3 : u8 {
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
}  // namespace encoding

#endif  // DECODER_HPP
