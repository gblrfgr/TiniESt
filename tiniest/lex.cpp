#include "lex.hpp"
#include <cstddef>
#include <tiniest/lex.hpp>

es::unicode_char es::next_utf8(const std::uint8_t *&cursor) noexcept {
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

void es::put_utf8(es::unicode_char chr, std::uint8_t *&cursor,
                  std::size_t &space_remaining) noexcept {
  if (cursor == nullptr || space_remaining == 0 || chr > 0x10FFFF) {
    return;
  }
  if (chr >= 0x10000) {
    if (space_remaining < 4) {
      return;
    }
    *cursor = 0b11110000 | (chr >> 18);
    cursor++;
    *cursor = 0b10000000 | ((chr >> 12) & 0b00111111);
    cursor++;
    *cursor = 0b10000000 | ((chr >> 6) & 0b00111111);
    cursor++;
    *cursor = 0b10000000 | (chr & 0b00111111);
    cursor++;
    space_remaining -= 4;
  } else if (chr >= 0x800) {
    if (space_remaining < 3) {
      return;
    }
    *cursor = 0b11100000 | (chr >> 12);
    cursor++;
    *cursor = 0b10000000 | ((chr >> 6) & 0b00111111);
    cursor++;
    *cursor = 0b10000000 | (chr & 0b00111111);
    cursor++;
    space_remaining -= 3;
  } else if (chr >= 0x80) {
    if (space_remaining < 2) {
      return;
    }
    *cursor = 0b11000000 | (chr >> 6);
    cursor++;
    *cursor = 0b10000000 | (chr & 0b00111111);
    cursor++;
    space_remaining -= 2;
  } else {
    *cursor = chr;
    cursor++;
    space_remaining--;
  }
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