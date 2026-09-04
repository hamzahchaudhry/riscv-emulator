#include "decoder.h"

#include <bit>
#include <optional>

#include "bit_utils.h"
#include "instruction.h"

namespace rv32i_emu {

namespace {

enum class Opcode : u8 {
  kRegister = 0b0110011,
  kImmediate = 0b0010011,
  kLoad = 0b0000011,
  kStore = 0b0100011,
  kBranch = 0b1100011,
  kJal = 0b1101111,
  kJalr = 0b1100111,
  kLui = 0b0110111,
  kAuipc = 0b0010111,
  kSystem = 0b1110011
};

enum class RegisterFunct3 : u8 {
  kAddSub = 0x0,
  kXor = 0x4,
  kOr = 0x6,
  kAnd = 0x7,
  kSll = 0x1,
  kSrlSra = 0x5,
  kSlt = 0x2,
  kSltu = 0x3
};

enum class RegisterFunct7 : u8 {
  kAdd = 0x00,
  kSub = 0x20,
  kXor = 0x00,
  kOr = 0x00,
  kAnd = 0x00,
  kSll = 0x00,
  kSrl = 0x00,
  kSra = 0x20,
  kSlt = 0x00,
  kSltu = 0x00
};

enum class ImmediateFunct3 : u8 {
  kAddi = 0x0,
  kSlli = 0x1,
  kSlti = 0x2,
  kSltiu = 0x3,
  kXori = 0x4,
  kSrliSrai = 0x5,
  kOri = 0x6,
  kAndi = 0x7
};

enum class ImmediateFunct7 : u8 { kSlli = 0x00, kSrli = 0x00, kSrai = 0x20 };

enum class LoadFunct3 : u8 {
  kLb = 0x0,
  kLh = 0x1,
  kLw = 0x2,
  kLbu = 0x4,
  kLhu = 0x5
};

enum class BranchFunct3 : u8 {
  kBeq = 0x0,
  kBne = 0x1,
  kBlt = 0x4,
  kBge = 0x5,
  kBltu = 0x6,
  kBgeu = 0x7
};

enum class StoreFunct3 : u8 { kSb = 0x0, kSh = 0x1, kSw = 0x2 };

enum class SystemFunct3 : u8 { kEcall = 0x0, kEbreak = 0x0 };

std::optional<RegisterInstruction> DecodeRegister(u32 raw) {
  const u8 rd = static_cast<u8>(Bits<7, 11>(raw));
  const auto funct3 = static_cast<RegisterFunct3>(Bits<12, 14>(raw));
  const u8 rs1 = static_cast<u8>(Bits<15, 19>(raw));
  const u8 rs2 = static_cast<u8>(Bits<20, 24>(raw));
  const auto funct7 = static_cast<RegisterFunct7>(Bits<25, 31>(raw));

  const auto make = [rd, rs1, rs2](RegisterOp opcode) {
    return RegisterInstruction{
        .opcode = opcode,
        .rd = rd,
        .rs1 = rs1,
        .rs2 = rs2,
    };
  };

  switch (funct3) {
    case RegisterFunct3::kAddSub:
      if (funct7 == RegisterFunct7::kAdd) return make(RegisterOp::kAdd);
      if (funct7 == RegisterFunct7::kSub) return make(RegisterOp::kSub);
      return std::nullopt;

    case RegisterFunct3::kXor:
      if (funct7 == RegisterFunct7::kXor) return make(RegisterOp::kXor);
      return std::nullopt;

    case RegisterFunct3::kOr:
      if (funct7 == RegisterFunct7::kOr) return make(RegisterOp::kOr);
      return std::nullopt;

    case RegisterFunct3::kAnd:
      if (funct7 == RegisterFunct7::kAnd) return make(RegisterOp::kAnd);
      return std::nullopt;

    case RegisterFunct3::kSll:
      if (funct7 == RegisterFunct7::kSll) return make(RegisterOp::kSll);
      return std::nullopt;

    case RegisterFunct3::kSrlSra:
      if (funct7 == RegisterFunct7::kSrl) return make(RegisterOp::kSrl);
      if (funct7 == RegisterFunct7::kSra) return make(RegisterOp::kSra);
      return std::nullopt;

    case RegisterFunct3::kSlt:
      if (funct7 == RegisterFunct7::kSlt) return make(RegisterOp::kSlt);
      return std::nullopt;

    case RegisterFunct3::kSltu:
      if (funct7 == RegisterFunct7::kSltu) return make(RegisterOp::kSltu);
      return std::nullopt;

    default:
      return std::nullopt;
  }
}

std::optional<ImmediateInstruction> DecodeImmediate(u32 raw) {
  const u8 rd = static_cast<u8>(Bits<7, 11>(raw));
  const auto funct3 = static_cast<ImmediateFunct3>(Bits<12, 14>(raw));
  const u8 rs1 = static_cast<u8>(Bits<15, 19>(raw));
  const i32 imm = std::bit_cast<i32>(SignExtend<12>(Bits<20, 31>(raw)));

  const auto make = [rd, rs1, imm](ImmediateOp opcode) {
    return ImmediateInstruction{
        .opcode = opcode,
        .rd = rd,
        .rs1 = rs1,
        .imm = imm,
    };
  };

  switch (funct3) {
    case ImmediateFunct3::kAddi:
      return make(ImmediateOp::kAddi);

    case ImmediateFunct3::kXori:
      return make(ImmediateOp::kXori);

    case ImmediateFunct3::kOri:
      return make(ImmediateOp::kOri);

    case ImmediateFunct3::kAndi:
      return make(ImmediateOp::kAndi);

    case ImmediateFunct3::kSlti:
      return make(ImmediateOp::kSlti);

    case ImmediateFunct3::kSltiu:
      return make(ImmediateOp::kSltiu);

    default:
      return std::nullopt;
  }
}

std::optional<ShiftImmediateInstruction> DecodeShiftImmediate(u32 raw) {
  const auto funct3 = static_cast<ImmediateFunct3>(Bits<12, 14>(raw));
  const auto funct7 = static_cast<ImmediateFunct7>(Bits<25, 31>(raw));

  const u8 rd = static_cast<u8>(Bits<7, 11>(raw));
  const u8 rs1 = static_cast<u8>(Bits<15, 19>(raw));
  const u8 shamt = static_cast<u8>(Bits<20, 24>(raw));

  const auto make = [rd, rs1, shamt](ShiftImmediateOp opcode) {
    return ShiftImmediateInstruction{
        .opcode = opcode,
        .rd = rd,
        .rs1 = rs1,
        .shamt = shamt,
    };
  };

  switch (funct3) {
    case ImmediateFunct3::kSlli:
      if (funct7 != ImmediateFunct7::kSlli) return std::nullopt;
      return make(ShiftImmediateOp::kSlli);

    case ImmediateFunct3::kSrliSrai: {
      switch (funct7) {
        case ImmediateFunct7::kSrli:
          return make(ShiftImmediateOp::kSrli);

        case ImmediateFunct7::kSrai:
          return make(ShiftImmediateOp::kSrai);

        default:
          return std::nullopt;
      }

      default:
        return std::nullopt;
    }
  }
}

std::optional<LoadInstruction> DecodeLoad(u32 raw) {
  const u8 rd = static_cast<u8>(Bits<7, 11>(raw));
  const auto funct3 = static_cast<LoadFunct3>(Bits<12, 14>(raw));
  const u8 base = static_cast<u8>(Bits<15, 19>(raw));
  const i32 offset = std::bit_cast<i32>(SignExtend<12>(Bits<20, 31>(raw)));

  const auto make = [rd, base, offset](LoadOp opcode) {
    return LoadInstruction{
        .opcode = opcode,
        .rd = rd,
        .base = base,
        .offset = offset,
    };
  };

  switch (funct3) {
    case LoadFunct3::kLb:
      return make(LoadOp::kLb);

    case LoadFunct3::kLh:
      return make(LoadOp::kLh);

    case LoadFunct3::kLw:
      return make(LoadOp::kLw);

    case LoadFunct3::kLbu:
      return make(LoadOp::kLbu);

    case LoadFunct3::kLhu:
      return make(LoadOp::kLhu);

    default:
      return std::nullopt;
  }
}

std::optional<StoreInstruction> DecodeStore(u32 raw) {
  const auto funct3 = static_cast<StoreFunct3>(Bits<12, 14>(raw));
  const u8 base = static_cast<u8>(Bits<15, 19>(raw));
  const u8 source = static_cast<u8>(Bits<20, 24>(raw));
  const u32 encoded_offset = (Bits<25, 31>(raw) << 5) | Bits<7, 11>(raw);
  const i32 offset = std::bit_cast<i32>(SignExtend<12>(encoded_offset));

  const auto make = [source, base, offset](StoreOp opcode) {
    return StoreInstruction{
        .opcode = opcode,
        .source = source,
        .base = base,
        .offset = offset,
    };
  };

  switch (funct3) {
    case StoreFunct3::kSb:
      return make(StoreOp::kSb);

    case StoreFunct3::kSh:
      return make(StoreOp::kSh);

    case StoreFunct3::kSw:
      return make(StoreOp::kSw);

    default:
      return std::nullopt;
  }
}

std::optional<BranchInstruction> DecodeBranch(u32 raw) {
  const auto funct3 = static_cast<BranchFunct3>(Bits<12, 14>(raw));
  const u8 rs1 = static_cast<u8>(Bits<15, 19>(raw));
  const u8 rs2 = static_cast<u8>(Bits<20, 24>(raw));
  const u32 encoded_offset = (Bits<31, 31>(raw) << 12) |
                             (Bits<7, 7>(raw) << 11) |
                             (Bits<25, 30>(raw) << 5) | (Bits<8, 11>(raw) << 1);
  const i32 offset = std::bit_cast<i32>(SignExtend<13>(encoded_offset));

  const auto make = [rs1, rs2, offset](BranchOp opcode) {
    return BranchInstruction{
        .opcode = opcode,
        .rs1 = rs1,
        .rs2 = rs2,
        .offset = offset,
    };
  };

  switch (funct3) {
    case BranchFunct3::kBeq:
      return make(BranchOp::kBeq);

    case BranchFunct3::kBne:
      return make(BranchOp::kBne);

    case BranchFunct3::kBlt:
      return make(BranchOp::kBlt);

    case BranchFunct3::kBge:
      return make(BranchOp::kBge);

    case BranchFunct3::kBltu:
      return make(BranchOp::kBltu);

    case BranchFunct3::kBgeu:
      return make(BranchOp::kBgeu);

    default:
      return std::nullopt;
  }
}

std::optional<UpperInstruction> DecodeUpper(u32 raw, UpperOp opcode) {
  const u8 rd = static_cast<u8>(Bits<7, 11>(raw));
  const u32 imm = Bits<12, 31>(raw) << 12;

  const auto make = [rd, imm](UpperOp opcode) {
    return UpperInstruction{
        .opcode = opcode,
        .rd = rd,
        .imm = imm,
    };
  };

  switch (opcode) {
    case UpperOp::kLui:
      return make(UpperOp::kLui);

    case UpperOp::kAuipc:
      return make(UpperOp::kAuipc);

    default:
      return std::nullopt;
  }
}

std::optional<Jal> DecodeJal(u32 raw) {
  const u8 rd = static_cast<u8>(Bits<7, 11>(raw));
  const u32 encoded_offset =
      (Bits<31, 31>(raw) << 20) | (Bits<12, 19>(raw) << 12) |
      (Bits<20, 20>(raw) << 11) | (Bits<21, 30>(raw) << 1);
  const i32 offset = std::bit_cast<i32>(SignExtend<21>(encoded_offset));

  return Jal{
      .rd = rd,
      .offset = offset,
  };
}

std::optional<Jalr> DecodeJalr(u32 raw) {
  const u8 rd = static_cast<u8>(Bits<7, 11>(raw));
  const u8 funct3 = static_cast<u8>(Bits<12, 14>(raw));
  const u8 base = static_cast<u8>(Bits<15, 19>(raw));
  const i32 offset = std::bit_cast<i32>(SignExtend<12>(Bits<20, 31>(raw)));

  if (funct3 == 0x0)
    return Jalr{
        .rd = rd,
        .base = base,
        .offset = offset,
    };
  return std::nullopt;
}

std::optional<SystemInstruction> DecodeSystem(u32 raw) {
  const u8 funct3 = static_cast<u8>(Bits<12, 14>(raw));
  const i32 imm = std::bit_cast<i32>(SignExtend<12>(Bits<20, 31>(raw)));

  if (funct3 == 0x0) switch (imm) {
      case 0x0:
        return SystemInstruction{.opcode = SystemOp::kEcall};

      case 0x1:
        return SystemInstruction{.opcode = SystemOp::kEbreak};

      default:
        return std::nullopt;
    }
  return std::nullopt;
}

}  // namespace

std::optional<Instruction> Decode(u32 raw) {
  const auto opcode = static_cast<Opcode>(Bits<0, 6>(raw));

  switch (opcode) {
    case Opcode::kRegister:
      return DecodeRegister(raw);

    case Opcode::kImmediate: {
      const u32 funct3 = Bits<12, 14>(raw);

      if (funct3 == 0x1 || funct3 == 0x5) return DecodeShiftImmediate(raw);

      return DecodeImmediate(raw);
    }

    case Opcode::kLoad:
      return DecodeLoad(raw);

    case Opcode::kStore:
      return DecodeStore(raw);

    case Opcode::kBranch:
      return DecodeBranch(raw);

    case Opcode::kLui:
      return DecodeUpper(raw, UpperOp::kLui);

    case Opcode::kAuipc:
      return DecodeUpper(raw, UpperOp::kAuipc);

    case Opcode::kJal:
      return DecodeJal(raw);

    case Opcode::kJalr:
      return DecodeJalr(raw);

    case Opcode::kSystem:
      return DecodeSystem(raw);

    default:
      return std::nullopt;
  }
}

}  // namespace rv32i_emu
