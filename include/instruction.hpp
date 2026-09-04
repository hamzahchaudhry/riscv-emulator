#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include "types.hpp"

#include <variant>

enum class RegisterOp : u8 {
  Add,
  Sub,
  Xor,
  Or,
  And,
  Sll,
  Srl,
  Sra,
  Slt,
  Sltu
};

enum class ImmediateOp : u8 {
  Addi,
  Xori,
  Ori,
  Andi,
  Slti,
  Sltiu
};

enum class ShiftImmediateOp : u8 {
  Slli,
  Srli,
  Srai,
};

enum class LoadOp : u8 {
  Lb,
  Lh,
  Lw,
  Lbu,
  Lhu
};

enum class StoreOp : u8 {
  Sb,
  Sh,
  Sw
};
enum class BranchOp : u8 {
  Beq,
  Bne,
  Blt,
  Bge,
  Bltu,
  Bgeu
};

enum class JumpOp : u8 {
  Jal,
  Jalr
};

enum class UpperOp : u8 {
  Lui,
  Auipc
};

enum class SystemOp : u8 {
  Ecall,
  Ebreak
};

struct RegisterInstruction {
  RegisterOp op;
  u8 rd;
  u8 rs1;
  u8 rs2;
};

struct ImmediateInstruction {
  ImmediateOp op;
  u8 rd;
  u8 rs1;
  i32 imm;
};

struct ShiftImmediateInstruction {
  ShiftImmediateOp op;
  u8 rd;
  u8 rs1;
  u8 shamt;
};

struct LoadInstruction {
  LoadOp op;
  u8 rd;
  u8 base;
  i32 offset;
};

struct StoreInstruction {
  StoreOp op;
  u8 source;
  u8 base;
  i32 offset;
};

struct BranchInstruction {
  BranchOp op;
  u8 rs1;
  u8 rs2;
  i32 offset;
};

struct Jal {
  u8 rd;
  i32 offset;
};

struct Jalr {
  u8 rd;
  u8 base;
  i32 offset;
};

struct UpperInstruction {
  UpperOp op;
  u8 rd;
  u32 imm;
};

struct SystemInstruction {
  SystemOp op;
};

using Instruction = std::variant<
    RegisterInstruction,
    ImmediateInstruction,
    ShiftImmediateInstruction,
    LoadInstruction,
    StoreInstruction,
    BranchInstruction,
    UpperInstruction,
    Jal,
    Jalr,
    SystemInstruction>;

#endif  // INSTRUCTION_HPP
