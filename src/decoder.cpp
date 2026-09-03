#include "decoder.hpp"
#include "instruction.hpp"

#include <optional>

i32 sign_extend(u32 imm, u32 n) {
  u8 sign = (imm >> (n - 1)) & 1;
  return imm | (-sign << n);
}

std::optional<Instruction> decode(u32 instr) {
  const auto opcode = static_cast<Opcode>(instr & 0x7F);
  switch (opcode) {
    case Opcode::R: {
      const auto rd = static_cast<u8>((instr >> 7) & 0x1F);
      const auto funct3 = static_cast<R_Funct3>((instr >> 12) & 0x7);
      const auto rs1 = static_cast<u8>((instr >> 15) & 0x1F);
      const auto rs2 = static_cast<u8>((instr >> 20) & 0x1F);
      const auto funct7 = static_cast<R_Funct7>((instr >> 25) & 0x7F);
      return RType{opcode, rd, rs1, funct3, rs2, funct7};
    }

    case Opcode::I: {
      const auto rd = static_cast<u8>((instr >> 7) & 0x1F);
      const auto funct3 = static_cast<I_Funct3>((instr >> 12) & 0x7);
      const auto rs1 = static_cast<u8>((instr >> 15) & 0x1F);
      const auto imm = sign_extend((instr >> 20) & 0xFFF, 12);
      return IType{opcode, rd, funct3, rs1, imm};
    }

    case Opcode::I_LOAD: {
      const auto rd = static_cast<u8>((instr >> 7) & 0x1F);
      const auto funct3 = static_cast<I_LOAD_Funct3>((instr >> 12) & 0x7);
      const auto rs1 = static_cast<u8>((instr >> 15) & 0x1F);
      const auto imm = sign_extend((instr >> 20) & 0xFFF, 12);
      return IType{opcode, rd, funct3, rs1, imm};
    }

    case Opcode::I_ENV: {
      const auto rd = static_cast<u8>((instr >> 7) & 0x1F);
      const auto funct3 = static_cast<I_ENV_Funct3>((instr >> 12) & 0x7);
      const auto rs1 = static_cast<u8>((instr >> 15) & 0x1F);
      const auto imm = sign_extend((instr >> 20) & 0xFFF, 12);
      return IType{opcode, rd, funct3, rs1, imm};
    }

    case Opcode::S: {
      const auto funct3 = static_cast<S_Funct3>((instr >> 12) & 0x7);
      const auto rs1 = static_cast<u8>((instr >> 15) & 0x1F);
      const auto rs2 = static_cast<u8>((instr >> 20) & 0x1F);

      const auto imm11_5 = ((instr >> 25) & 0x7F) << 5;
      const auto imm4_0 = (instr >> 7) & 0x1F;
      const auto imm = sign_extend(imm11_5 | imm4_0, 12);

      return SType{opcode, funct3, rs1, rs2, imm};
    }

    case Opcode::B: {
      const auto funct3 = static_cast<B_Funct3>((instr >> 12) & 0x7);
      const auto rs1 = static_cast<u8>((instr >> 15) & 0x1F);
      const auto rs2 = static_cast<u8>((instr >> 20) & 0x1F);

      const auto imm12 = ((instr >> 31) & 0x1) << 12;
      const auto imm11 = ((instr >> 7) & 0x1) << 11;
      const auto imm10_5 = ((instr >> 25) & 0x3F) << 5;
      const auto imm4_1 = ((instr >> 8) & 0xF) << 1;
      const auto imm = sign_extend(imm12 | imm11 | imm10_5 | imm4_1, 13);

      return BType{opcode, funct3, rs1, rs2, imm};
    }

    case Opcode::J_JAL: {
      const auto rd = static_cast<u8>((instr >> 7) & 0x1F);

      const auto imm20 = ((instr >> 31) & 0x1) << 20;
      const auto imm19_12 = ((instr >> 12) & 0xFF) << 12;
      const auto imm11 = ((instr >> 20) & 0x1) << 11;
      const auto imm10_1 = ((instr >> 21) & 0x3FF) << 1;
      const auto imm = sign_extend(imm20 | imm19_12 | imm11 | imm10_1, 21);

      return JType{opcode, rd, imm};
    }

    case Opcode::U_LUI:
    case Opcode::U_AUIPC: {
      const auto rd = static_cast<u8>((instr >> 7) & 0x1F);
      const auto imm = sign_extend((instr >> 11), 33);
      return UType{opcode, rd, imm};
    }

    default:
      return std::nullopt;
  }
}
