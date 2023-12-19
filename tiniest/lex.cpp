#include "lex.hpp"
#include <tiniest/lex.hpp>

es::unicode_char es::next_utf8(char *&cursor) {
  if (*cursor & 0b1000000 == 0) {
    return static_cast<es::unicode_char>(*cursor++);
  } else if (*cursor & 0b11100000 == 0b11000000) {
    es::unicode_char accumulator = *cursor & 0b00011111;
    accumulator <<= 6;
    accumulator |= *(cursor + 1) & 0b00111111;
    cursor += 2;
    return accumulator;
  } else if (*cursor & 0b11110000 == 0b11100000) {
    es::unicode_char accumulator = *cursor & 0b00001111;
    accumulator <<= 6;
    accumulator |= *(cursor + 1) & 0b00111111;
    accumulator <<= 6;
    accumulator |= *(cursor + 2) & 0b00111111;
    cursor += 3;
    return accumulator;
  } else if (*cursor & 0b11111000 == 0b11110000) {
    es::unicode_char accumulator = *cursor & 0b00000111;
    accumulator <<= 6;
    accumulator |= *(cursor + 1) & 0b00111111;
    accumulator <<= 6;
    accumulator |= *(cursor + 2) & 0b00111111;
    accumulator <<= 6;
    accumulator |= *(cursor + 3) & 0b00111111;
    cursor += 4;
    return accumulator;
  } else {
    return 0;
  }
}