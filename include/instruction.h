#ifndef RV32I_EMU_INSTRUCTION_H_
#define RV32I_EMU_INSTRUCTION_H_

#include <variant>

#include "types.h"

namespace rv32i_emu {

enum class RegisterOp : u8 { kAdd, kSub, kXor, kOr, kAnd, kSll, kSrl, kSra, kSlt, kSltu };

enum class ImmediateOp : u8 { kAddi, kXori, kOri, kAndi, kSlti, kSltiu };

enum class ShiftImmediateOp : u8 {
  kSlli,
  kSrli,
  kSrai,
};

enum class LoadOp : u8 { kLb, kLh, kLw, kLbu, kLhu };

enum class StoreOp : u8 { kSb, kSh, kSw };
enum class BranchOp : u8 { kBeq, kBne, kBlt, kBge, kBltu, kBgeu };

enum class JumpOp : u8 { kJal, kJalr };

enum class UpperOp : u8 { kLui, kAuipc };

enum class SystemOp : u8 { kEcall, kEbreak };

struct RegisterInstruction {
  RegisterOp opcode;
  u8 rd;
  u8 rs1;
  u8 rs2;
};

struct ImmediateInstruction {
  ImmediateOp opcode;
  u8 rd;
  u8 rs1;
  i32 imm;
};

struct ShiftImmediateInstruction {
  ShiftImmediateOp opcode;
  u8 rd;
  u8 rs1;
  u8 shamt;
};

struct LoadInstruction {
  LoadOp opcode;
  u8 rd;
  u8 base;
  i32 offset;
};

struct StoreInstruction {
  StoreOp opcode;
  u8 source;
  u8 base;
  i32 offset;
};

struct BranchInstruction {
  BranchOp opcode;
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
  UpperOp opcode;
  u8 rd;
  u32 imm;
};

struct Fence {};

struct SystemInstruction {
  SystemOp opcode;
};

using Instruction =
    std::variant<RegisterInstruction, ImmediateInstruction, ShiftImmediateInstruction,
                 LoadInstruction, StoreInstruction, BranchInstruction, UpperInstruction, Jal, Jalr,
                 Fence, SystemInstruction>;

}  // namespace rv32i_emu

#endif  // RV32I_EMU_INSTRUCTION_H_
