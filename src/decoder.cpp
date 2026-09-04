#include "decoder.hpp"
#include "bit_utils.hpp"
#include "instruction.hpp"

#include <optional>

std::optional<Instruction> decode(u32 raw) {
  using encoding::Opcode;

  const auto opcode = static_cast<Opcode>(bits<0, 6>(raw));

  switch (opcode) {
    case Opcode::R:
      return decode_register(raw);

    case Opcode::I: {
      const u32 funct3 = bits<12, 14>(raw);

      if (funct3 == 0x1 || funct3 == 0x5)
        return decode_shift_immediate(raw);

      return decode_immediate(raw);
    }

    case Opcode::I_LOAD:
      return decode_load(raw);

    case Opcode::S:
      return decode_store(raw);

    case Opcode::B:
      return decode_branch(raw);

    case Opcode::U_LUI:
      return decode_upper(raw, UpperOp::Lui);

    case Opcode::U_AUIPC:
      return decode_upper(raw, UpperOp::Auipc);

    case Opcode::J_JAL:
      return decode_jal(raw);

    case Opcode::I_JALR:
      return decode_jalr(raw);

    case Opcode::I_SYS:
      return decode_system(raw);

    default:
      return std::nullopt;
  }
}

std::optional<RegisterInstruction> decode_register(const u32 raw) {
  using encoding::R_Funct3;
  using encoding::R_Funct7;

  const u8 rd = static_cast<u8>(bits<7, 11>(raw));
  const R_Funct3 funct3 = static_cast<R_Funct3>(bits<12, 14>(raw));
  const u8 rs1 = static_cast<u8>(bits<15, 19>(raw));
  const u8 rs2 = static_cast<u8>(bits<20, 24>(raw));
  const R_Funct7 funct7 = static_cast<R_Funct7>(bits<25, 31>(raw));

  const auto make = [rd, rs1, rs2](RegisterOp op) {
    return RegisterInstruction{
        .op = op,
        .rd = rd,
        .rs1 = rs1,
        .rs2 = rs2,
    };
  };

  switch (funct3) {
    case R_Funct3::ADD_SUB:
      if (funct7 == R_Funct7::ADD)
        return make(RegisterOp::Add);
      if (funct7 == R_Funct7::SUB)
        return make(RegisterOp::Sub);
      return std::nullopt;

    case R_Funct3::XOR:
      if (funct7 == R_Funct7::XOR)
        return make(RegisterOp::Xor);
      return std::nullopt;

    case R_Funct3::OR:
      if (funct7 == R_Funct7::OR)
        return make(RegisterOp::Or);
      return std::nullopt;

    case R_Funct3::AND:
      if (funct7 == R_Funct7::AND)
        return make(RegisterOp::And);
      return std::nullopt;

    case R_Funct3::SLL:
      if (funct7 == R_Funct7::SLL)
        return make(RegisterOp::Sll);
      return std::nullopt;

    case R_Funct3::SRL_SRA:
      if (funct7 == R_Funct7::SRL)
        return make(RegisterOp::Srl);
      if (funct7 == R_Funct7::SRA)
        return make(RegisterOp::Sra);
      return std::nullopt;

    case R_Funct3::SLT:
      if (funct7 == R_Funct7::SLT)
        return make(RegisterOp::Slt);
      return std::nullopt;

    case R_Funct3::SLTU:
      if (funct7 == R_Funct7::SLTU)
        return make(RegisterOp::Sltu);
      return std::nullopt;

    default:
      return std::nullopt;
  }
}

std::optional<ImmediateInstruction> decode_immediate(const u32 raw) {
  using encoding::I_Funct3;
  using encoding::I_Funct7;

  const u8 rd = static_cast<u8>(bits<7, 11>(raw));
  const I_Funct3 funct3 = static_cast<I_Funct3>(bits<12, 14>(raw));
  const u8 rs1 = static_cast<u8>(bits<15, 19>(raw));
  const i32 imm = SignExtend<12>(bits<20, 31>(raw));

  const auto make = [rd, rs1, imm](ImmediateOp op) {
    return ImmediateInstruction{
        .op = op,
        .rd = rd,
        .rs1 = rs1,
        .imm = imm,
    };
  };

  switch (funct3) {
    case I_Funct3::ADDI:
      return make(ImmediateOp::Addi);

    case I_Funct3::XORI:
      return make(ImmediateOp::Xori);

    case I_Funct3::ORI:
      return make(ImmediateOp::Ori);

    case I_Funct3::ANDI:
      return make(ImmediateOp::Andi);

    case I_Funct3::SLTI:
      return make(ImmediateOp::Slti);

    case I_Funct3::SLTIU:
      return make(ImmediateOp::Sltiu);

    default:
      return std::nullopt;
  }
}

std::optional<ShiftImmediateInstruction> decode_shift_immediate(const u32 raw) {
  using encoding::I_Funct3;
  using encoding::I_Funct7;

  const I_Funct3 funct3 = static_cast<I_Funct3>(bits<12, 14>(raw));
  const I_Funct7 funct7 = static_cast<I_Funct7>(bits<25, 31>(raw));

  const u8 rd = static_cast<u8>(bits<7, 11>(raw));
  const u8 rs1 = static_cast<u8>(bits<15, 19>(raw));
  const u8 shamt = static_cast<u8>(bits<20, 24>(raw));

  const auto make = [rd, rs1, shamt](ShiftImmediateOp op) {
    return ShiftImmediateInstruction{
        .op = op,
        .rd = rd,
        .rs1 = rs1,
        .shamt = shamt,
    };
  };

  switch (funct3) {
    case I_Funct3::SLLI:
      if (funct7 != I_Funct7::SLLI)
        return std::nullopt;
      return make(ShiftImmediateOp::Slli);

    case I_Funct3::SRLI_SRAI: {
      switch (funct7) {
        case I_Funct7::SRLI:
          return make(ShiftImmediateOp::Srli);

        case I_Funct7::SRAI:
          return make(ShiftImmediateOp::Srai);

        default:
          return std::nullopt;
      }
    }
  }

  return std::nullopt;
}

std::optional<LoadInstruction> decode_load(const u32 raw) {
  using encoding::I_LOAD_Funct3;

  const u8 rd = static_cast<u8>(bits<7, 11>(raw));
  const I_LOAD_Funct3 funct3 = static_cast<I_LOAD_Funct3>(bits<12, 14>(raw));
  const u8 base = static_cast<u8>(bits<15, 19>(raw));
  const i32 offset = SignExtend<12>(bits<20, 31>(raw));

  const auto make = [rd, base, offset](LoadOp op) {
    return LoadInstruction{
        .op = op,
        .rd = rd,
        .base = base,
        .offset = offset,
    };
  };

  switch (funct3) {
    case I_LOAD_Funct3::LB:
      return make(LoadOp::Lb);

    case I_LOAD_Funct3::LH:
      return make(LoadOp::Lh);

    case I_LOAD_Funct3::LW:
      return make(LoadOp::Lw);

    case I_LOAD_Funct3::LBU:
      return make(LoadOp::Lbu);

    case I_LOAD_Funct3::LHU:
      return make(LoadOp::Lhu);

    default:
      return std::nullopt;
  }
}

std::optional<StoreInstruction> decode_store(const u32 raw) {
  using encoding::S_Funct3;

  const S_Funct3 funct3 = static_cast<S_Funct3>(bits<12, 14>(raw));
  const u8 base = static_cast<u8>(bits<15, 19>(raw));
  const u8 source = static_cast<u8>(bits<20, 24>(raw));
  const i32 offset = SignExtend<12>(static_cast<i32>(bits<25, 31>(raw)) << 5) | static_cast<i32>(bits<7, 11>(raw));

  const auto make = [source, base, offset](StoreOp op) {
    return StoreInstruction{
        .op = op,
        .source = source,
        .base = base,
        .offset = offset,
    };
  };

  switch (funct3) {
    case S_Funct3::SB:
      return make(StoreOp::Sb);

    case S_Funct3::SH:
      return make(StoreOp::Sh);

    case S_Funct3::SW:
      return make(StoreOp::Sw);

    default:
      return std::nullopt;
  }
}

std::optional<BranchInstruction> decode_branch(const u32 raw) {
  using encoding::B_Funct3;

  const B_Funct3 funct3 = static_cast<B_Funct3>(bits<12, 14>(raw));
  const u8 rs1 = static_cast<u8>(bits<15, 19>(raw));
  const u8 rs2 = static_cast<u8>(bits<20, 24>(raw));
  const u32 encoded_offset =
      (bits<31, 31>(raw) << 12) |
      (bits<7, 7>(raw) << 11) |
      (bits<25, 30>(raw) << 5) |
      (bits<8, 11>(raw) << 1);
  const i32 offset = SignExtend<13>(encoded_offset);

  const auto make = [rs1, rs2, offset](BranchOp op) {
    return BranchInstruction{
        .op = op,
        .rs1 = rs1,
        .rs2 = rs2,
        .offset = offset,
    };
  };

  switch (funct3) {
    case B_Funct3::BEQ:
      return make(BranchOp::Beq);

    case B_Funct3::BNE:
      return make(BranchOp::Bne);

    case B_Funct3::BLT:
      return make(BranchOp::Blt);

    case B_Funct3::BGE:
      return make(BranchOp::Bge);

    case B_Funct3::BLTU:
      return make(BranchOp::Bltu);

    case B_Funct3::BGEU:
      return make(BranchOp::Bgeu);

    default:
      return std::nullopt;
  }
}

std::optional<UpperInstruction> decode_upper(const u32 raw, UpperOp op) {
  const u8 rd = static_cast<u8>(bits<7, 11>(raw));
  const u32 imm = bits<12, 31>(raw) << 12;

  const auto make = [rd, imm](UpperOp op) {
    return UpperInstruction{
        .op = op,
        .rd = rd,
        .imm = imm,
    };
  };

  switch (op) {
    case UpperOp::Lui:
      return make(UpperOp::Lui);

    case UpperOp::Auipc:
      return make(UpperOp::Auipc);

    default:
      return std::nullopt;
  }
}

std::optional<Jal> decode_jal(const u32 raw) {
  const u8 rd = static_cast<u8>(bits<7, 11>(raw));
  const u32 encoded_offset =
      (bits<31, 31>(raw) << 20) |
      (bits<12, 19>(raw) << 12) |
      (bits<20, 20>(raw) << 11) |
      (bits<21, 30>(raw) << 1);
  const i32 offset = SignExtend<21>(encoded_offset);

  return Jal{
      .rd = rd,
      .offset = offset,
  };
}

std::optional<Jalr> decode_jalr(const u32 raw) {
  const u8 rd = static_cast<u8>(bits<7, 11>(raw));
  const u8 funct3 = static_cast<u8>(bits<12, 14>(raw));
  const u8 base = static_cast<u8>(bits<15, 19>(raw));
  const i32 offset = SignExtend<12>(bits<20, 31>(raw));

  if (funct3 == 0x0)
    return Jalr{
        .rd = rd,
        .base = base,
        .offset = offset,
    };
  return std::nullopt;
}

std::optional<SystemInstruction> decode_system(const u32 raw) {
  const u8 funct3 = static_cast<u8>(bits<12, 14>(raw));
  const i32 imm = SignExtend<12>(bits<20, 31>(raw));

  if (funct3 == 0x0)
    switch (imm) {
      case 0x0:
        return SystemInstruction{.op = SystemOp::Ecall};

      case 0x1:
        return SystemInstruction{.op = SystemOp::Ebreak};

      default:
        return std::nullopt;
    }
  return std::nullopt;
}
