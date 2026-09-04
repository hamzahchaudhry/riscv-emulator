#ifndef RV32I_EMU_BIT_UTILS_H_
#define RV32I_EMU_BIT_UTILS_H_

#include "types.h"

namespace rv32i_emu {

// template <unsigned size, typename Type>
// static auto SignExtend(const Type val) {
//   static_assert(std::is_integral_v<Type>, "Only integral types are
//   supported");
//   // static_assert(size > 0 && size < (sizeof(Type) * CHAR_BIT), "Invalid
//   size value"); using SignedType = std::make_signed_t<Type>; struct {
//     SignedType val : size;
//   } holder = {.val = static_cast<SignedType>(val)};
//   // Compiler takes care of sign-extension of the field with the specified
//   bit-length. return static_cast<SignedType>(holder.val);
// }

template <u32 width>
constexpr u32 SignExtend(u32 value) noexcept {
  static_assert(width > 0);
  static_assert(width < 32);

  constexpr u32 sign_bit = u32{1} << (width - 1);
  constexpr u32 mask = (u32{1} << width) - 1;

  value &= mask;
  return (value ^ sign_bit) - sign_bit;
}

template <u32 start, u32 end>
constexpr u32 Bits(u32 value) noexcept {
  static_assert(start <= end);
  static_assert(end < 32);

  constexpr u8 width = end - start + 1;

  if constexpr (width == 32) return value;

  const u32 mask = (u32{1} << width) - 1;
  return (value >> start) & mask;
}

}  // namespace rv32i_emu

#endif  // RV32I_EMU_BIT_UTILS_H_
