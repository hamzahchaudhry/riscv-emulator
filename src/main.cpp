#include "cpu.hpp"
#include "memory.hpp"
#include <print>

int main() {
  CPU cpu;
  Memory mem("tests/test.bin");

  cpu.emulate_cycle(mem);
  if (cpu.read_reg(1) != 5)
    std::println("failed on addi x1, x0, 5: {}", cpu.read_reg(1));
  else
    std::println("passed 1");

  cpu.emulate_cycle(mem);
  if (cpu.read_reg(2) != 7)
    std::println("failed on addi x2, x0, 7");
  else
    std::println("passed 2");

  cpu.emulate_cycle(mem);
  if (cpu.read_reg(3) != 12)
    std::println("failed on add x3, x1, x2");
  else
    std::println("passed 3");
}
