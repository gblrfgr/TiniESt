#include <tiniest/lex.hpp>
#include "catch.hpp"


TEST_CASE("next_utf8 returns correct decoded character") {
  SECTION("Single-byte UTF-8 character") {
    const char *input = "A";
    const es::unicode_char expected = 'A';
    REQUIRE(next_utf8(input) == expected);
  }

  SECTION("Two-byte UTF-8 character") {
    const char *input = u8"¢";
    const es::unicode_char expected = 0x00A2;
    REQUIRE(next_utf8(input) == expected);
  }

  SECTION("Three-byte UTF-8 character") {
    const char *input = u8"€";
    const es::unicode_char expected = 0x20AC;
    REQUIRE(next_utf8(input) == expected);
  }

  SECTION("Four-byte UTF-8 character") {
    const char *input = u8"𐍈";
    const es::unicode_char expected = 0x10348;
    REQUIRE(next_utf8(input) == expected);
  }
}
