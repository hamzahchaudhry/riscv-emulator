#include "cpu.hpp"

#include <optional>
#include <stdexcept>
#include <variant>

std::uint32_t CPU::read_reg(std::uint8_t index) const {
  if (index >= x.size())
    throw std::out_of_range("invalid register index");
  return (index == 0) ? 0 : x[index];
}

bool CPU::write_reg(std::uint8_t index, std::uint32_t val) {
  if (index > 0 && index < x.size()) {
    x[index] = val;
    return true;
  }
  return false;
}

void CPU::emulate_cycle(Memory &memory) {
  std::uint32_t instr = memory.read_addr(PC);
  const auto decoded_instr = decode(instr);
  if (decoded_instr)
    execute(*decoded_instr);
}

std::optional<Instruction> CPU::decode(const std::uint32_t &instr) {
  const auto opcode = static_cast<Opcode>(instr & 0x7F);
  switch (opcode) {
    case Opcode::R: {
      const auto rd = static_cast<std::uint8_t>((instr >> 7) & 0x1F);
      const auto funct3 = static_cast<R_Funct3>((instr >> 12) & 0x7);
      const auto rs1 = static_cast<std::uint8_t>((instr >> 15) & 0x1F);
      const auto rs2 = static_cast<std::uint8_t>((instr >> 20) & 0x1F);
      const auto funct7 = static_cast<R_Funct7>((instr >> 25) & 0x7F);
      return RType{opcode, rd, funct3, rs1, rs2, funct7};
    }
    case Opcode::I: {
      const auto rd = static_cast<std::uint8_t>((instr >> 7) & 0x1F);
      const auto funct3 = static_cast<I_Funct3>((instr >> 12) & 0x7);
      const auto rs1 = static_cast<std::uint8_t>((instr >> 15) & 0x1F);
      const auto imm = static_cast<std::int32_t>((instr >> 20) & 0xFFF);
      return IType{opcode, rd, funct3, rs1, imm};
    }
    default:
      return std::nullopt;
  }
}

void CPU::execute(const Instruction &instr) {
  if (std::holds_alternative<RType>(instr)) {
    const auto &decoded_instr = std::get<RType>(instr);
    switch (decoded_instr.funct3) {
      case R_Funct3::ADD_SUB: {
        switch (decoded_instr.funct7) {
          case R_Funct7::ADD:
            write_reg(decoded_instr.rd, read_reg(decoded_instr.rs1) + read_reg(decoded_instr.rs2));
            PC += 4;
            break;
          default:
            return;
        }
        break;
        default:
          break;
      }
    }
  } else if (std::holds_alternative<IType>(instr)) {
    const auto &decoded_instr = std::get<IType>(instr);
    switch (decoded_instr.funct3) {
      case I_Funct3::ADDI:
        write_reg(decoded_instr.rd,
                  read_reg(decoded_instr.rs1) + sign_extend(decoded_instr.imm, 12));
        PC += 4;
        break;
      default:
        break;
    }
  }
}

std::int32_t CPU::sign_extend(std::uint32_t imm, std::uint32_t n) {
  std::uint8_t sign = (imm >> (n - 1)) & 1;
  return imm | (-sign << n);
}
