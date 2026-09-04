#include "hart.hpp"
#include "bit_utils.hpp"
#include "decoder.hpp"
#include "instruction.hpp"

#include <expected>
#include <variant>

std::expected<void, Hart::Trap> Hart::step(Memory &memory) {
  const auto raw = memory.read_word(pc_);
  const auto instr = decode(raw);

  if (!instr)
    return std::unexpected(Trap::IllegalInstruction);

  const auto next_pc = execute(*instr, memory);

  if (!next_pc)
    return std::unexpected(next_pc.error());

  pc_ = *next_pc;
  return {};
}

std::expected<u32, Hart::Trap> Hart::execute(const Instruction &instr, Memory &memory) {
  const auto sequential = [this](std::expected<void, Trap> result) -> std::expected<u32, Trap> {
    if (!result)
      return std::unexpected(result.error());
    return pc_ + 4;
  };

  if (const auto *r = std::get_if<RegisterInstruction>(&instr))
    return sequential(execute_register(*r));

  if (const auto *i = std::get_if<ImmediateInstruction>(&instr))
    return sequential(execute_immediate(*i));

  if (const auto *i = std::get_if<ShiftImmediateInstruction>(&instr))
    return sequential(execute_shift_immediate(*i));

  if (const auto *i = std::get_if<LoadInstruction>(&instr))
    return sequential(execute_load(*i, memory));

  if (const auto *s = std::get_if<StoreInstruction>(&instr))
    return sequential(execute_store(*s, memory));

  if (const auto *b = std::get_if<BranchInstruction>(&instr))
    return execute_branch(*b);

  if (const auto *u = std::get_if<UpperInstruction>(&instr))
    return sequential(execute_upper(*u));

  if (const auto *j = std::get_if<Jal>(&instr))
    return execute_jal(*j);

  if (const auto *i = std::get_if<Jalr>(&instr))
    return execute_jalr(*i);

  if (const auto *i = std::get_if<SystemInstruction>(&instr))
    return sequential(execute_system(*i));

  return std::unexpected(Trap::IllegalInstruction);
}

std::expected<void, Hart::Trap> Hart::execute_register(const RegisterInstruction &instr) {
  const u32 rs1 = registers_.read(instr.rs1);
  const u32 rs2 = registers_.read(instr.rs2);
  u32 result;

  switch (instr.op) {
    case RegisterOp::Add:
      result = rs1 + rs2;
      break;

    case RegisterOp::Sub:
      result = rs1 - rs2;
      break;

    case RegisterOp::Xor:
      result = rs1 ^ rs2;
      break;

    case RegisterOp::Or:
      result = rs1 | rs2;
      break;

    case RegisterOp::And:
      result = rs1 & rs2;
      break;

    case RegisterOp::Sll:
      result = rs1 << (rs2 & 0x1F);
      break;

    case RegisterOp::Srl:
      result = rs1 >> (rs2 & 0x1F);
      break;

    case RegisterOp::Sra:
      result = static_cast<u32>(static_cast<i32>(rs1) >> (rs2 & 0x1F));
      break;

    case RegisterOp::Slt:
      result = static_cast<i32>(rs1) < static_cast<i32>(rs2);
      break;

    case RegisterOp::Sltu:
      result = rs1 < rs2;
      break;

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }

  registers_.write(instr.rd, result);
  return {};
}

std::expected<void, Hart::Trap> Hart::execute_immediate(const ImmediateInstruction &instr) {
  const u32 rs1 = registers_.read(instr.rs1);
  u32 result;

  switch (instr.op) {
    case ImmediateOp::Addi:
      result = rs1 + instr.imm;
      break;

    case ImmediateOp::Xori:
      result = rs1 ^ instr.imm;
      break;

    case ImmediateOp::Ori:
      result = rs1 | instr.imm;
      break;

    case ImmediateOp::Andi:
      result = rs1 & instr.imm;
      break;

    case ImmediateOp::Slti:
      result = static_cast<i32>(rs1) < instr.imm;
      break;

    case ImmediateOp::Sltiu:
      result = (static_cast<u32>(rs1) < static_cast<u32>(instr.imm)) ? 1 : 0;
      break;

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }

  registers_.write(instr.rd, result);
  return {};
}

std::expected<void, Hart::Trap> Hart::execute_shift_immediate(const ShiftImmediateInstruction &instr) {
  const u32 rs1 = registers_.read(instr.rs1);
  u32 result;

  switch (instr.op) {
    case ShiftImmediateOp::Slli:
      result = rs1 << instr.shamt;
      break;

    case ShiftImmediateOp::Srli:
      result = rs1 >> instr.shamt;
      break;

    case ShiftImmediateOp::Srai:
      result = static_cast<u32>(static_cast<i32>(rs1) >> instr.shamt);
      break;

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }

  registers_.write(instr.rd, result);
  return {};
}

std::expected<void, Hart::Trap> Hart::execute_load(const LoadInstruction &instr, Memory &memory) {
  const u32 base = registers_.read(instr.base);
  const u32 address = add_offset(base, instr.offset);
  u32 result;

  switch (instr.op) {
    case LoadOp::Lb:
      result = SignExtend<8>(memory.read_byte(address));
      break;

    case LoadOp::Lh:
      result = SignExtend<16>(memory.read_half(address));
      break;

    case LoadOp::Lw:
      result = memory.read_word(address);
      break;

    case LoadOp::Lbu:
      result = memory.read_byte(address);
      break;

    case LoadOp::Lhu:
      result = memory.read_half(address);
      break;

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }

  registers_.write(instr.rd, result);
  return {};
}

std::expected<void, Hart::Trap> Hart::execute_store(const StoreInstruction &instr, Memory &memory) {
  const u32 source = registers_.read(instr.source);
  const u32 base = registers_.read(instr.base);
  const u32 address = add_offset(base, instr.offset);

  switch (instr.op) {
    case StoreOp::Sb:
      memory.write_byte(address, static_cast<u8>(source));
      return {};

    case StoreOp::Sh:
      memory.write_half(address, static_cast<u16>(source));
      return {};

    case StoreOp::Sw:
      memory.write_word(address, source);
      return {};

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }
}
std::expected<u32, Hart::Trap> Hart::execute_branch(const BranchInstruction &instr) {
  const u32 rs1 = registers_.read(instr.rs1);
  const u32 rs2 = registers_.read(instr.rs2);

  bool taken = false;

  switch (instr.op) {
    case BranchOp::Beq:
      taken = static_cast<i32>(rs1) == static_cast<i32>(rs2);
      break;

    case BranchOp::Bne:
      taken = static_cast<i32>(rs1) != static_cast<i32>(rs2);
      break;

    case BranchOp::Blt:
      taken = static_cast<i32>(rs1) < static_cast<i32>(rs2);
      break;

    case BranchOp::Bge:
      taken = static_cast<i32>(rs1) >= static_cast<i32>(rs2);
      break;

    case BranchOp::Bltu:
      taken = rs1 < rs2;
      break;

    case BranchOp::Bgeu:
      taken = rs1 >= rs2;
      break;

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }

  return add_offset(pc_, taken ? instr.offset : 4);
}

std::expected<u32, Hart::Trap> Hart::execute_jal(const Jal &instr) {
  registers_.write(instr.rd, pc_ + 4);
  return add_offset(pc_, instr.offset);
}

std::expected<u32, Hart::Trap> Hart::execute_jalr(const Jalr &instr) {
  const u32 base = registers_.read(instr.base);
  registers_.write(instr.rd, pc_ + 4);
  return add_offset(base, instr.offset) & ~u32{1};
}

std::expected<void, Hart::Trap> Hart::execute_upper(const UpperInstruction &instr) {
  switch (instr.op) {
    case UpperOp::Lui:
      registers_.write(instr.rd, instr.imm);
      return {};

    case UpperOp::Auipc:
      registers_.write(instr.rd, pc_ + instr.imm);
      return {};

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }
}

std::expected<void, Hart::Trap> Hart::execute_system(const SystemInstruction &instr) {
  switch (instr.op) {
    case SystemOp::Ecall:
      return std::unexpected(Trap::EnvironmentCall);

    case SystemOp::Ebreak:
      return std::unexpected(Trap::EnvironmentBreak);

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }
}
