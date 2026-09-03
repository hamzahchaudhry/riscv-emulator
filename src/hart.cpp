#include "hart.hpp"
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

  if (const auto *r = std::get_if<RType>(&instr))
    return sequential(execute_r_type(*r));

  if (const auto *u = std::get_if<UType>(&instr))
    return sequential(execute_u_type(*u));

  if (const auto *s = std::get_if<SType>(&instr))
    return sequential(execute_s_type(*s, memory));

  if (const auto *b = std::get_if<BType>(&instr))
    return execute_b_type(*b);

  if (const auto *j = std::get_if<JType>(&instr))
    return execute_j_type(*j);

  if (const auto *i = std::get_if<IType>(&instr)) {
    if (i->opcode == Opcode::I_LOAD)
      return sequential(execute_load(*i, memory));
    return sequential(execute_i_type(*i));
  }

  return std::unexpected(Trap::IllegalInstruction);
}

std::expected<void, Hart::Trap> Hart::execute_r_type(const RType &instr) {
  switch (instr.funct3) {
    case R_Funct3::ADD_SUB:
      switch (instr.funct7) {
        case R_Funct7::ADD:
          registers_.write(instr.rd, registers_.read(instr.rs1) + registers_.read(instr.rs2));
          return {};

        case R_Funct7::SUB:
          registers_.write(instr.rd, registers_.read(instr.rs1) - registers_.read(instr.rs2));
          return {};

        default:
          return std::unexpected(Trap::IllegalInstruction);
      }
      return std::unexpected(Trap::IllegalInstruction);

    case R_Funct3::XOR:
      if (instr.funct7 != R_Funct7::XOR)
        return std::unexpected(Trap::IllegalInstruction);
      registers_.write(instr.rd, registers_.read(instr.rs1) ^ registers_.read(instr.rs2));
      return {};

    case R_Funct3::OR:
      if (instr.funct7 != R_Funct7::OR)
        return std::unexpected(Trap::IllegalInstruction);
      registers_.write(instr.rd, registers_.read(instr.rs1) | registers_.read(instr.rs2));
      return {};

    case R_Funct3::AND:
      if (instr.funct7 != R_Funct7::AND)
        return std::unexpected(Trap::IllegalInstruction);
      registers_.write(instr.rd, registers_.read(instr.rs1) & registers_.read(instr.rs2));
      return {};

    case R_Funct3::SLL:
      if (instr.funct7 != R_Funct7::SLL)
        return std::unexpected(Trap::IllegalInstruction);
      registers_.write(instr.rd, registers_.read(instr.rs1) << registers_.read(instr.rs2));
      return {};

    case R_Funct3::SRL_SRA:
      switch (instr.funct7) {
        case R_Funct7::SRL:
          registers_.write(instr.rd, registers_.read(instr.rs1) >> (registers_.read(instr.rs2) & 0x1F));
          return {};

        case R_Funct7::SRA:
          registers_.write(instr.rd, static_cast<i32>(registers_.read(instr.rs1)) >> static_cast<i32>((registers_.read(instr.rs2) & 0x1F)));
          return {};

        default:
          return std::unexpected(Trap::IllegalInstruction);
      }
      return std::unexpected(Trap::IllegalInstruction);

    case R_Funct3::SLT:
      if (instr.funct7 != R_Funct7::SLT)
        return std::unexpected(Trap::IllegalInstruction);
      registers_.write(instr.rd, (registers_.read(instr.rs1) < registers_.read(instr.rs2)) ? 1 : 0);
      return {};

    case R_Funct3::SLTU:
      if (instr.funct7 != R_Funct7::SLTU)
        return std::unexpected(Trap::IllegalInstruction);
      registers_.write(instr.rd, (registers_.read(instr.rs1) < registers_.read(instr.rs2)) ? 1 : 0);
      return {};

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }
}

std::expected<void, Hart::Trap> Hart::execute_i_type(const IType &instr) {
  const u32 imm = static_cast<u32>(instr.imm);
  switch (std::get<I_Funct3>(instr.funct3)) {
    case I_Funct3::ADDI:
      registers_.write(instr.rd, registers_.read(instr.rs1) + imm);
      return {};

    case I_Funct3::XORI:
      registers_.write(instr.rd, registers_.read(instr.rs1) ^ imm);
      return {};

    case I_Funct3::ORI:
      registers_.write(instr.rd, registers_.read(instr.rs1) | imm);
      return {};

    case I_Funct3::ANDI:
      registers_.write(instr.rd, registers_.read(instr.rs1) & imm);
      return {};

    case I_Funct3::SLLI: {
      const auto funct7 = static_cast<I_Funct7>((static_cast<u32>(instr.imm) >> 5) & 0x7F);
      const auto shamt = static_cast<u32>(instr.imm) & 0x1F;

      if (funct7 != I_Funct7::SLLI)
        return std::unexpected(Trap::IllegalInstruction);
      registers_.write(instr.rd, registers_.read(instr.rs1) << (static_cast<u32>(instr.imm) & shamt));
      return {};
    }

    case I_Funct3::SRLI_SRAI: {
      const auto funct7 = static_cast<I_Funct7>((static_cast<u32>(instr.imm) >> 5) & 0x7F);
      const auto shamt = static_cast<u32>(instr.imm) & 0x1F;

      switch (funct7) {
        case I_Funct7::SRLI:
          registers_.write(instr.rd, registers_.read(instr.rs1) >> shamt);
          return {};

        case I_Funct7::SRAI:
          registers_.write(instr.rd, static_cast<u32>(static_cast<i32>(registers_.read(instr.rs1)) >> shamt));
          return {};

        default:
          return std::unexpected(Trap::IllegalInstruction);
      }
    }

    case I_Funct3::SLTI:
      registers_.write(instr.rd, (registers_.read(instr.rs1) < instr.imm) ? 1 : 0);
      return {};

    case I_Funct3::SLTIU:
      registers_.write(instr.rd, (static_cast<u32>(registers_.read(instr.rs1)) < static_cast<u32>(instr.imm)) ? 1 : 0);
      return {};

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }
}

std::expected<u32, Hart::Trap> Hart::execute_b_type(const BType &instr) {
  switch (instr.funct3) {
    case B_Funct3::BEQ:
      return add_offset(pc_, (static_cast<i32>(registers_.read(instr.rs1)) == static_cast<i32>(registers_.read(instr.rs2))) ? instr.imm : 4);

    case B_Funct3::BNE:
      return add_offset(pc_, (static_cast<i32>(registers_.read(instr.rs1)) != static_cast<i32>(registers_.read(instr.rs2))) ? instr.imm : 4);

    case B_Funct3::BLT:
      return add_offset(pc_, (static_cast<i32>(registers_.read(instr.rs1)) < static_cast<i32>(registers_.read(instr.rs2))) ? instr.imm : 4);

    case B_Funct3::BGE:
      return add_offset(pc_, (static_cast<i32>(registers_.read(instr.rs1)) >= static_cast<i32>(registers_.read(instr.rs2))) ? instr.imm : 4);

    case B_Funct3::BLTU:
      return add_offset(pc_, (registers_.read(instr.rs1) < registers_.read(instr.rs2)) ? instr.imm : 4);

    case B_Funct3::BGEU:
      return add_offset(pc_, (registers_.read(instr.rs1) >= registers_.read(instr.rs2)) ? instr.imm : 4);

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }
}

std::expected<u32, Hart::Trap> Hart::execute_j_type(const JType &instr) {
  registers_.write(instr.rd, pc_ + 4);
  return add_offset(pc_, instr.imm);
}

std::expected<void, Hart::Trap> Hart::execute_u_type(const UType &instr) {
  switch (instr.opcode) {
    case Opcode::U_LUI:
      registers_.write(instr.rd, instr.imm);
      return {};

    case Opcode::U_AUIPC:
      registers_.write(instr.rd, pc_ + instr.imm);
      return {};

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }
}

std::expected<void, Hart::Trap> Hart::execute_load(const IType &instr, Memory &memory) {
  switch (std::get<I_LOAD_Funct3>(instr.funct3)) {
    case I_LOAD_Funct3::LB:
      registers_.write(instr.rd, memory.read_byte(registers_.read(instr.rs1) + instr.imm));
      return {};

    case I_LOAD_Funct3::LH:
      registers_.write(instr.rd, memory.read_half(registers_.read(instr.rs1) + instr.imm));
      return {};

    case I_LOAD_Funct3::LW:
      registers_.write(instr.rd, memory.read_word(registers_.read(instr.rs1) + instr.imm));
      return {};

    case I_LOAD_Funct3::LBU:
      registers_.write(instr.rd, memory.read_byte(registers_.read(instr.rs1) + static_cast<u32>(instr.imm)));
      return {};

    case I_LOAD_Funct3::LHU:
      registers_.write(instr.rd, memory.read_half(registers_.read(instr.rs1) + static_cast<u32>(instr.imm)));
      return {};

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }
}

std::expected<void, Hart::Trap> Hart::execute_s_type(const SType &instr, Memory &memory) {
  const u32 address = add_offset(registers_.read(instr.rs1), instr.imm);
  const u32 value = registers_.read(instr.rs2);

  switch (instr.funct3) {
    case S_Funct3::SB:
      memory.write_byte(address, static_cast<u8>(value));
      return {};

    case S_Funct3::SH:
      memory.write_half(address, static_cast<u16>(value));
      return {};

    case S_Funct3::SW:
      memory.write_word(address, value);
      return {};

    default:
      return std::unexpected(Trap::IllegalInstruction);
  }
}
