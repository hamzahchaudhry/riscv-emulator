#include "cpu.hpp"

#include <stdexcept>

word_t CPU::read_reg(byte_t index) const {
  if (index >= x.size())
    throw std::out_of_range("invalid register index");
  return (index == 0) ? 0 : x[index];
}

bool CPU::write_reg(byte_t index, word_t val) {
  if (index > 0 && index < x.size()) {
    x[index] = val;
    return true;
  }
  return false;
}

void CPU::emulate_cycle(Memory &memory) {
  word_t instruction = memory.read_addr(PC);
  execute(instruction);
}

void CPU::execute(word_t instruction) {
  word_t opcode = instruction & 0x7F;

  if (opcode == 0x13)
    execute_i_type(instruction);

  if (opcode == 0x33)
    execute_r_type(instruction);

  PC += 4;
}

void CPU::execute_i_type(word_t instruction) {
  byte_t rd = (instruction >> 7) & 0x1F;
  byte_t rs1 = (instruction >> 15) & 0x1F;

  write_reg(rd, read_reg(rs1) + sign_extend(instruction));
}

void CPU::execute_r_type(word_t instruction) {
  byte_t rd = (instruction >> 7) & 0x1F;
  byte_t rs1 = (instruction >> 15) & 0x1F;
  byte_t rs2 = (instruction >> 20) & 0x1F;

  write_reg(rd, read_reg(rs1) + read_reg(rs2));
}

word_t CPU::sign_extend(word_t instruction) {
  word_t opcode = instruction & 0x7F;

  if (opcode == 0b0010011) {
    word_t imm = instruction >> 20;

    if (imm & 0x800) {
      imm |= 0xFFFFF000;
    }

    return imm;
  }

  return 0;
}
