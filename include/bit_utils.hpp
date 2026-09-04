#ifndef BIT_UTILS_HPP
#define BIT_UTILS_HPP

#include "types.hpp"

#include <climits>
#include <type_traits>

template <unsigned size, typename Type>
static auto SignExtend(const Type val) {
  static_assert(std::is_integral_v<Type>, "Only integral types are supported");
  // static_assert(size > 0 && size < (sizeof(Type) * CHAR_BIT), "Invalid size value");
  using SignedType = std::make_signed_t<Type>;
  struct {
    SignedType val : size;
  } holder = {.val = static_cast<SignedType>(val)};
  // Compiler takes care of sign-extension of the field with the specified bit-length.
  return static_cast<SignedType>(holder.val);
}

template <u32 start, u32 end>
constexpr u32 bits(u32 value) {
  static_assert(start <= end);
  static_assert(end < 32);

  constexpr u8 width = end - start + 1;

  if constexpr (width == 32)
    return value;

  const u32 mask = (u32{1} << width) - 1;
  return (value >> start) & mask;
}

#endif  // BIT_UTILS_HPP
