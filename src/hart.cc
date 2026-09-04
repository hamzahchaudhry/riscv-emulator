#include "hart.h"

#include <bit>
#include <expected>
#include <variant>

#include "bit_utils.h"
#include "decoder.h"
#include "instruction.h"

namespace rv32i_emu {

std::expected<void, Hart::Trap> Hart::Step(Memory& memory) {
  const auto raw = memory.ReadWord(pc_);

  if (!raw) return std::unexpected(Trap::kInstructionAccessFault);

  const auto instr = Decode(*raw);

  if (!instr) return std::unexpected(Trap::kIllegalInstruction);

  const auto next_pc = Execute(*instr, memory);

  if (!next_pc) return std::unexpected(next_pc.error());

  pc_ = *next_pc;
  return {};
}

std::expected<u32, Hart::Trap> Hart::Execute(const Instruction& instr,
                                             Memory& memory) {
  const auto sequential =
      [this](std::expected<void, Trap> result) -> std::expected<u32, Trap> {
    if (!result) return std::unexpected(result.error());
    return pc_ + 4;
  };

  if (const auto* r = std::get_if<RegisterInstruction>(&instr))
    return sequential(ExecuteRegister(*r));

  if (const auto* i = std::get_if<ImmediateInstruction>(&instr))
    return sequential(ExecuteImmediate(*i));

  if (const auto* i = std::get_if<ShiftImmediateInstruction>(&instr))
    return sequential(ExecuteShiftImmediate(*i));

  if (const auto* i = std::get_if<LoadInstruction>(&instr))
    return sequential(ExecuteLoad(*i, memory));

  if (const auto* s = std::get_if<StoreInstruction>(&instr))
    return sequential(ExecuteStore(*s, memory));

  if (const auto* b = std::get_if<BranchInstruction>(&instr))
    return ExecuteBranch(*b);

  if (const auto* u = std::get_if<UpperInstruction>(&instr))
    return sequential(ExecuteUpper(*u));

  if (const auto* j = std::get_if<Jal>(&instr)) return ExecuteJal(*j);

  if (const auto* i = std::get_if<Jalr>(&instr)) return ExecuteJalr(*i);

  if (const auto* i = std::get_if<SystemInstruction>(&instr))
    return sequential(ExecuteSystem(*i));

  return std::unexpected(Trap::kIllegalInstruction);
}

std::expected<void, Hart::Trap> Hart::ExecuteRegister(
    const RegisterInstruction& instr) {
  const u32 rs1 = registers_.ReadRegister(instr.rs1);
  const u32 rs2 = registers_.ReadRegister(instr.rs2);
  u32 result = 0;

  switch (instr.opcode) {
    case RegisterOp::kAdd:
      result = rs1 + rs2;
      break;

    case RegisterOp::kSub:
      result = rs1 - rs2;
      break;

    case RegisterOp::kXor:
      result = rs1 ^ rs2;
      break;

    case RegisterOp::kOr:
      result = rs1 | rs2;
      break;

    case RegisterOp::kAnd:
      result = rs1 & rs2;
      break;

    case RegisterOp::kSll:
      result = rs1 << (rs2 & 0x1F);
      break;

    case RegisterOp::kSrl:
      result = rs1 >> (rs2 & 0x1F);
      break;

    case RegisterOp::kSra:
      result = static_cast<u32>(static_cast<i32>(rs1) >> (rs2 & 0x1F));
      break;

    case RegisterOp::kSlt:
      result = static_cast<i32>(rs1) < static_cast<i32>(rs2) ? 1 : 0;
      break;

    case RegisterOp::kSltu:
      result = rs1 < rs2 ? 1 : 0;
      break;

    default:
      return std::unexpected(Trap::kIllegalInstruction);
  }

  registers_.WriteRegister(instr.rd, result);
  return {};
}

std::expected<void, Hart::Trap> Hart::ExecuteImmediate(
    const ImmediateInstruction& instr) {
  const u32 rs1 = registers_.ReadRegister(instr.rs1);
  const u32 imm = std::bit_cast<u32>(instr.imm);
  u32 result = 0;

  switch (instr.opcode) {
    case ImmediateOp::kAddi:
      result = rs1 + imm;
      break;

    case ImmediateOp::kXori:
      result = rs1 ^ imm;
      break;

    case ImmediateOp::kOri:
      result = rs1 | imm;
      break;

    case ImmediateOp::kAndi:
      result = rs1 & imm;
      break;

    case ImmediateOp::kSlti:
      result = std::bit_cast<i32>(rs1) < instr.imm ? 1 : 0;
      break;

    case ImmediateOp::kSltiu:
      result = rs1 < imm ? 1 : 0;
      break;

    default:
      return std::unexpected(Trap::kIllegalInstruction);
  }

  registers_.WriteRegister(instr.rd, result);
  return {};
}

std::expected<void, Hart::Trap> Hart::ExecuteShiftImmediate(
    const ShiftImmediateInstruction& instr) {
  const u32 rs1 = registers_.ReadRegister(instr.rs1);
  u32 result = 0;

  switch (instr.opcode) {
    case ShiftImmediateOp::kSlli:
      result = rs1 << instr.shamt;
      break;

    case ShiftImmediateOp::kSrli:
      result = rs1 >> instr.shamt;
      break;

    case ShiftImmediateOp::kSrai:
      result = static_cast<u32>(static_cast<i32>(rs1) >> instr.shamt);
      break;

    default:
      return std::unexpected(Trap::kIllegalInstruction);
  }

  registers_.WriteRegister(instr.rd, result);
  return {};
}

std::expected<void, Hart::Trap> Hart::ExecuteLoad(const LoadInstruction& instr,
                                                  Memory& memory) {
  const u32 base = registers_.ReadRegister(instr.base);
  const u32 address = AddOffset(base, instr.offset);
  u32 result = 0;

  switch (instr.opcode) {
    case LoadOp::kLb: {
      const auto value = memory.ReadByte(address);
      if (!value) return std::unexpected(Trap::kLoadAccessFault);

      result = SignExtend<8>(*value);
      break;
    }

    case LoadOp::kLh: {
      const auto value = memory.ReadHalf(address);
      if (!value) return std::unexpected(Trap::kLoadAccessFault);

      result = SignExtend<16>(*value);
      break;
    }

    case LoadOp::kLw: {
      const auto value = memory.ReadWord(address);
      if (!value) return std::unexpected(Trap::kLoadAccessFault);

      result = *value;
      break;
    }

    case LoadOp::kLbu: {
      const auto value = memory.ReadByte(address);
      if (!value) return std::unexpected(Trap::kLoadAccessFault);

      result = *value;
      break;
    }

    case LoadOp::kLhu: {
      const auto value = memory.ReadHalf(address);
      if (!value) return std::unexpected(Trap::kLoadAccessFault);

      result = *value;
      break;
    }

    default:
      return std::unexpected(Trap::kIllegalInstruction);
  }

  registers_.WriteRegister(instr.rd, result);
  return {};
}

std::expected<void, Hart::Trap> Hart::ExecuteStore(
    const StoreInstruction& instr, Memory& memory) {
  const u32 source = registers_.ReadRegister(instr.source);
  const u32 base = registers_.ReadRegister(instr.base);
  const u32 address = AddOffset(base, instr.offset);

  switch (instr.opcode) {
    case StoreOp::kSb: {
      const auto err = memory.WriteByte(address, static_cast<u8>(source));
      if (!err) return std::unexpected(Trap::kStoreAccessFault);
      return {};
    }

    case StoreOp::kSh: {
      const auto err = memory.WriteHalf(address, static_cast<u16>(source));
      if (!err) return std::unexpected(Trap::kStoreAccessFault);
      return {};
    }

    case StoreOp::kSw: {
      const auto err = memory.WriteWord(address, source);
      if (!err) return std::unexpected(Trap::kStoreAccessFault);
      return {};
    }

    default:
      return std::unexpected(Trap::kIllegalInstruction);
  }
}

std::expected<u32, Hart::Trap> Hart::ExecuteBranch(
    const BranchInstruction& instr) {
  const u32 rs1 = registers_.ReadRegister(instr.rs1);
  const u32 rs2 = registers_.ReadRegister(instr.rs2);

  bool taken = false;

  switch (instr.opcode) {
    case BranchOp::kBeq:
      taken = static_cast<i32>(rs1) == static_cast<i32>(rs2);
      break;

    case BranchOp::kBne:
      taken = static_cast<i32>(rs1) != static_cast<i32>(rs2);
      break;

    case BranchOp::kBlt:
      taken = static_cast<i32>(rs1) < static_cast<i32>(rs2);
      break;

    case BranchOp::kBge:
      taken = static_cast<i32>(rs1) >= static_cast<i32>(rs2);
      break;

    case BranchOp::kBltu:
      taken = rs1 < rs2;
      break;

    case BranchOp::kBgeu:
      taken = rs1 >= rs2;
      break;

    default:
      return std::unexpected(Trap::kIllegalInstruction);
  }

  return AddOffset(pc_, taken ? instr.offset : 4);
}

std::expected<u32, Hart::Trap> Hart::ExecuteJal(const Jal& instr) {
  registers_.WriteRegister(instr.rd, pc_ + 4);
  return AddOffset(pc_, instr.offset);
}

std::expected<u32, Hart::Trap> Hart::ExecuteJalr(const Jalr& instr) {
  const u32 base = registers_.ReadRegister(instr.base);
  registers_.WriteRegister(instr.rd, pc_ + 4);
  return AddOffset(base, instr.offset) & ~u32{1};
}

std::expected<void, Hart::Trap> Hart::ExecuteUpper(
    const UpperInstruction& instr) {
  switch (instr.opcode) {
    case UpperOp::kLui:
      registers_.WriteRegister(instr.rd, instr.imm);
      return {};

    case UpperOp::kAuipc:
      registers_.WriteRegister(instr.rd, pc_ + instr.imm);
      return {};

    default:
      return std::unexpected(Trap::kIllegalInstruction);
  }
}

std::expected<void, Hart::Trap> Hart::ExecuteSystem(
    const SystemInstruction& instr) {
  switch (instr.opcode) {
    case SystemOp::kEcall:
      return std::unexpected(Trap::kEnvironmentCall);

    case SystemOp::kEbreak:
      return std::unexpected(Trap::kEnvironmentBreak);

    default:
      return std::unexpected(Trap::kIllegalInstruction);
  }
}

}  // namespace rv32i_emu
