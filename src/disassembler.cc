#include "disassembler.h"

#include <format>
#include <variant>

#include "instruction.h"

namespace rv32i_emu {

std::string Disassemble(const Instruction& instr) {
  if (const auto* r = std::get_if<RegisterInstruction>(&instr)) {
    switch (r->opcode) {
      case RegisterOp::kAdd:
        return std::format("add x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::kSub:
        return std::format("sub x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::kXor:
        return std::format("xor x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::kOr:
        return std::format("or x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::kAnd:
        return std::format("and x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::kSll:
        return std::format("sll x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::kSrl:
        return std::format("srl x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::kSra:
        return std::format("sra x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::kSlt:
        return std::format("slt x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      case RegisterOp::kSltu:
        return std::format("sltu x{}, x{}, x{}", r->rd, r->rs1, r->rs2);

      default:
        return "<illegal>";
    }
  }

  if (const auto* i = std::get_if<ImmediateInstruction>(&instr)) {
    switch (i->opcode) {
      case ImmediateOp::kAddi:
        return std::format("addi x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::kXori:
        return std::format("xori x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::kOri:
        return std::format("ori x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::kAndi:
        return std::format("andi x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::kSlti:
        return std::format("slti x{}, x{}, {}", i->rd, i->rs1, i->imm);

      case ImmediateOp::kSltiu:
        return std::format("sltiu x{}, x{}, {}", i->rd, i->rs1, i->imm);

      default:
        return "<illegal>";
    }
  }

  if (const auto* i = std::get_if<ShiftImmediateInstruction>(&instr)) {
    switch (i->opcode) {
      case ShiftImmediateOp::kSlli:
        return std::format("slli x{}, x{}, {}", i->rd, i->rs1, i->shamt);

      case ShiftImmediateOp::kSrli:
        return std::format("srli x{}, x{}, {}", i->rd, i->rs1, i->shamt);

      case ShiftImmediateOp::kSrai:
        return std::format("srai x{}, x{}, {}", i->rd, i->rs1, i->shamt);

      default:
        return "<illegal>";
    }
  }

  if (const auto* i = std::get_if<LoadInstruction>(&instr)) {
    switch (i->opcode) {
      case LoadOp::kLb:
        return std::format("lb x{}, {}(x{})", i->rd, i->offset, i->base);

      case LoadOp::kLh:
        return std::format("lh x{}, {}(x{})", i->rd, i->offset, i->base);

      case LoadOp::kLw:
        return std::format("lw x{}, {}(x{})", i->rd, i->offset, i->base);

      case LoadOp::kLbu:
        return std::format("lbu x{}, {}(x{})", i->rd, i->offset, i->base);

      case LoadOp::kLhu:
        return std::format("lhu x{}, {}(x{})", i->rd, i->offset, i->base);

      default:
        return "<illegal>";
    }
  }

  if (const auto* s = std::get_if<StoreInstruction>(&instr)) {
    switch (s->opcode) {
      case StoreOp::kSb:
        return std::format("sb x{}, {}(x{})", s->source, s->offset, s->base);

      case StoreOp::kSh:
        return std::format("sh x{}, {}(x{})", s->source, s->offset, s->base);

      case StoreOp::kSw:
        return std::format("sw x{}, {}(x{})", s->source, s->offset, s->base);

      default:
        return "<illegal>";
    }
  }

  if (const auto* b = std::get_if<BranchInstruction>(&instr)) {
    switch (b->opcode) {
      case BranchOp::kBeq:
        return std::format("beq x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::kBne:
        return std::format("bne x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::kBlt:
        return std::format("blt x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::kBge:
        return std::format("bge x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::kBltu:
        return std::format("bltu x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      case BranchOp::kBgeu:
        return std::format("bgeu x{}, x{}, {}", b->rs1, b->rs2, b->offset);

      default:
        return "<illegal>";
    }
  }

  if (const auto* j = std::get_if<Jal>(&instr))
    return std::format("jal x{}, {}", j->rd, j->offset);

  if (const auto* i = std::get_if<Jalr>(&instr))
    return std::format("jalr x{}, {}(x{})", i->rd, i->offset, i->base);

  if (const auto* u = std::get_if<UpperInstruction>(&instr)) {
    switch (u->opcode) {
      case UpperOp::kLui:
        return std::format("lui x{}, {}", u->rd, u->imm);

      case UpperOp::kAuipc:
        return std::format("auipc x{}, {}", u->rd, u->imm);

      default:
        return "<illegal>";
    }
  }

  if (const auto* i = std::get_if<SystemInstruction>(&instr)) {
    switch (i->opcode) {
      case SystemOp::kEcall:
        return "ecall";

      case SystemOp::kEbreak:
        return "ebreak";

      default:
        return "<illegal>";
    }
  }

  return "<unknown>";
}

}  // namespace rv32i_emu
