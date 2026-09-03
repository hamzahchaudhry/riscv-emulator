#include "disasm.hpp"
#include <format>

std::string disasm(const Instruction &instr) {
  if (const auto *r = std::get_if<RType>(&instr)) {
    switch (r->funct3) {
      case R_Funct3::ADD_SUB:
        switch (r->funct7) {
          case R_Funct7::ADD:
            return std::format("add x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

          case R_Funct7::SUB:
            return std::format("sub x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

          default:
            return "<illegal>";
        }
        return "<illegal>";

      case R_Funct3::XOR:
        if (r->funct7 != R_Funct7::XOR)
          return "<illegal>";
        return std::format("xor x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case R_Funct3::OR:
        if (r->funct7 != R_Funct7::OR)
          return "<illegal>";
        return std::format("or x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case R_Funct3::AND:
        if (r->funct7 != R_Funct7::AND)
          return "<illegal>";
        return std::format("and x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case R_Funct3::SLL:
        if (r->funct7 != R_Funct7::SLL)
          return "<illegal>";
        return std::format("sll x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case R_Funct3::SRL_SRA:
        switch (r->funct7) {
          case R_Funct7::SRL:
            return std::format("srl x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

          case R_Funct7::SRA:
            return std::format("sra x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

          default:
            return "<illegal>";
        }
        return "<illegal>";

      case R_Funct3::SLT:
        if (r->funct7 != R_Funct7::SLT)
          return "<illegal>";
        return std::format("slt x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case R_Funct3::SLTU:
        if (r->funct7 != R_Funct7::SLTU)
          return "<illegal>";
        return std::format("sltu x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      default:
        return "<illegal>";
    }

  } else if (const auto *i = std::get_if<IType>(&instr)) {
    if (const auto *funct3 = std::get_if<I_Funct3>(&i->funct3)) {
      switch (*funct3) {
        case I_Funct3::ADDI:
          return std::format("addi x{}, x{}, {}", i->rd, i->rs1, i->imm);

        case I_Funct3::XORI:
          return std::format("xori x{}, x{}, {}", i->rd, i->rs1, i->imm);

        case I_Funct3::ORI:
          return std::format("ori x{}, x{}, {}", i->rd, i->rs1, i->imm);

        case I_Funct3::ANDI:
          return std::format("andi x{}, x{}, {}", i->rd, i->rs1, i->imm);

        case I_Funct3::SLLI: {
          const auto funct7 = static_cast<I_Funct7>((static_cast<u32>(i->imm) >> 5) & 0x7F);
          const auto shamt = static_cast<u32>(i->imm) & 0x1F;

          if (funct7 != I_Funct7::SLLI)
            return "<illegal>";
          return std::format("slli x{}, x{}, {}", i->rd, i->rs1, shamt);
        }

        case I_Funct3::SRLI_SRAI: {
          const auto funct7 = static_cast<I_Funct7>((static_cast<u32>(i->imm) >> 5) & 0x7F);
          const auto shamt = static_cast<u32>(i->imm) & 0x1F;

          switch (funct7) {
            case I_Funct7::SRLI:
              return std::format("srli x{}, x{}, {}", i->rd, i->rs1, shamt);

            case I_Funct7::SRAI:
              return std::format("srai x{}, x{}, {}", i->rd, i->rs1, shamt);

            default:
              return "<illegal>";
          }
        }

        case I_Funct3::SLTI:
          return std::format("slti x{}, x{}, {}", i->rd, i->rs1, i->imm);

        case I_Funct3::SLTIU:
          return std::format("sltiu x{}, x{}, {}", i->rd, i->rs1, i->imm);

        default:
          return "<illegal>";
      }
    } else if (const auto *funct3 = std::get_if<I_LOAD_Funct3>(&i->funct3)) {
      switch (*funct3) {
        case I_LOAD_Funct3::LB:
          return std::format("lb x{}, {}(x{})", i->rd, i->imm, i->rs1);

        case I_LOAD_Funct3::LH:
          return std::format("lh x{}, {}(x{})", i->rd, i->imm, i->rs1);

        case I_LOAD_Funct3::LW:
          return std::format("lw x{}, {}(x{})", i->rd, i->imm, i->rs1);

        case I_LOAD_Funct3::LBU:
          return std::format("lbu x{}, {}(x{})", i->rd, i->imm, i->rs1);

        case I_LOAD_Funct3::LHU:
          return std::format("lhu x{}, {}(x{})", i->rd, i->imm, i->rs1);

        default:
          return "<illegal>";
      }
    }
  } else if (const auto *u = std::get_if<UType>(&instr)) {
    switch (u->opcode) {
      case Opcode::U_LUI:
        return std::format("lui x{}, {}", u->rd, u->imm);

      case Opcode::U_AUIPC:
        return std::format("auipc x{}, {}", u->rd, u->imm);

      default:
        return "<illegal>";
    }
  }

  return "<unknown>";
}
