#include "lex.hpp"
#include <cstddef>
#include <tiniest/lex.hpp>

es::unicode_char es::next_utf8(const char *&cursor) noexcept {
  if (cursor == nullptr) {
    return ~0;
  }
  es::unicode_char accumulator = 0;
  std::size_t n_extending;
  if ((*cursor & 0b10000000) == 0) {
    accumulator += *cursor;
    n_extending = 0;
  } else if ((*cursor & 0b11100000) == 0b11000000) {
    accumulator += *cursor & 0b00011111;
    n_extending = 1;
  } else if ((*cursor & 0b11110000) == 0b11100000) {
    accumulator += *cursor & 0b00001111;
    n_extending = 2;
  } else if ((*cursor & 0b11111000) == 0b11110000) {
    accumulator += *cursor & 0b00000111;
    n_extending = 3;
  } else {
    return ~0;
  }
  cursor++;
  for (std::size_t i = 0; i < n_extending; i++) {
    if ((*cursor & 0b11000000) != 0b10000000) {
      // defend against buffer underflow
      return ~0;
    }
    accumulator <<= 6;
    accumulator += *cursor & 0b00111111;
    cursor++;
  }
  return accumulator;
}

bool es::is_whitespace(es::unicode_char chr) noexcept {
  if (chr == 0x09 || chr == 0x0B || chr == 0x0C || chr == 0x20 || chr == 0xA0 ||
      chr == 0xFEFF) {
    // basic types mentioned in the spec
    return true;
  } else if (chr >= 0x2000 && chr <= 0x200A) {
    // various widths of spaces
    return true;
  } else if (chr == 0x1680 || chr == 0x202F || chr == 0x205F || chr == 0x3000) {
    // other unicode spaces
    return true;
  }
  return false;
}