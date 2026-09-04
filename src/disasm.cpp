#include "disasm.hpp"
#include "instruction.hpp"
#include <format>

std::string disasm(const Instruction &instr) {
  if (const auto *r = std::get_if<RegisterInstruction>(&instr)) {
    switch (r->op) {
      case RegisterOp::Add:
        return std::format("add x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::Sub:
        return std::format("sub x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::Xor:
        return std::format("xor x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::Or:
        return std::format("or x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::And:
        return std::format("and x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::Sll:
        return std::format("sll x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::Srl:
        return std::format("srl x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::Sra:
        return std::format("sra x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::Slt:
        return std::format("slt x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::Sltu:
        return std::format("sltu x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      default:
        return "<illegal>";
    }
  }

  if (const auto *i = std::get_if<ImmediateInstruction>(&instr)) {
    switch (i->op) {
      case ImmediateOp::Addi:
        return std::format("addi x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::Xori:
        return std::format("xori x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::Ori:
        return std::format("ori x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::Andi:
        return std::format("andi x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::Slti:
        return std::format("slti x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::Sltiu:
        return std::format("sltiu x{}, x{}, {}", i->rd, i->rs1, i->imm);

      default:
        return "<illegal>";
    }
  }

  if (const auto *i = std::get_if<ShiftImmediateInstruction>(&instr)) {
    switch (i->op) {
      case ShiftImmediateOp::Slli:
        return std::format("slli x{}, x{}, {}", i->rd, i->rs1, i->shamt);

      case ShiftImmediateOp::Srli:
        return std::format("srli x{}, x{}, {}", i->rd, i->rs1, i->shamt);

      case ShiftImmediateOp::Srai:
        return std::format("srai x{}, x{}, {}", i->rd, i->rs1, i->shamt);

      default:
        return "<illegal>";
    }
  }

  if (const auto *i = std::get_if<LoadInstruction>(&instr)) {
    switch (i->op) {
      case LoadOp::Lb:
        return std::format("lb x{}, {}(x{})", i->rd, i->offset, i->base);

      case LoadOp::Lh:
        return std::format("lh x{}, {}(x{})", i->rd, i->offset, i->base);

      case LoadOp::Lw:
        return std::format("lw x{}, {}(x{})", i->rd, i->offset, i->base);

      case LoadOp::Lbu:
        return std::format("lbu x{}, {}(x{})", i->rd, i->offset, i->base);

      case LoadOp::Lhu:
        return std::format("lhu x{}, {}(x{})", i->rd, i->offset, i->base);

      default:
        return "<illegal>";
    }
  }

  if (const auto *s = std::get_if<StoreInstruction>(&instr)) {
    switch (s->op) {
      case StoreOp::Sb:
        return std::format("sb x{}, {}(x{})", s->source, s->offset, s->base);

      case StoreOp::Sh:
        return std::format("sh x{}, {}(x{})", s->source, s->offset, s->base);

      case StoreOp::Sw:
        return std::format("sw x{}, {}(x{})", s->source, s->offset, s->base);

      default:
        return "<illegal>";
    }
  }

  if (const auto *b = std::get_if<BranchInstruction>(&instr)) {
    switch (b->op) {
      case BranchOp::Beq:
        return std::format("beq x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::Bne:
        return std::format("bne x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::Blt:
        return std::format("blt x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::Bge:
        return std::format("bge x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::Bltu:
        return std::format("bltu x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::Bgeu:
        return std::format("bgeu x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      default:
        return "<illegal>";
    }
  }

  if (const auto *j = std::get_if<Jal>(&instr))
    return std::format("jal x{}, {}", j->rd, j->offset);

  if (const auto *i = std::get_if<Jalr>(&instr))
    return std::format("jalr x{}, {}(x{})", i->rd, i->offset, i->base);

  if (const auto *u = std::get_if<UpperInstruction>(&instr)) {
    switch (u->op) {
      case UpperOp::Lui:
        return std::format("lui x{}, {}", u->rd, u->imm);

      case UpperOp::Auipc:
        return std::format("auipc x{}, {}", u->rd, u->imm);

      default:
        return "<illegal>";
    }
  }

  if (const auto *i = std::get_if<SystemInstruction>(&instr)) {
    switch (i->op) {
      case SystemOp::Ecall:
        return "ecall";

      case SystemOp::Ebreak:
        return "ebreak";

      default:
        return "<illegal>";
    }
  }

  return "<unknown>";
}
