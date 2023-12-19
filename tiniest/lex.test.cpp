#include <catch2/catch_test_macros.hpp>
#include <tiniest/lex.hpp>

TEST_CASE("next_utf8 behaves correctly") {
  SECTION("Single-byte UTF-8 character") {
    const char *input = "A";
    const es::unicode_char expected = 'A';
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Two-byte UTF-8 character") {
    const char *input = u8"¢";
    const es::unicode_char expected = 0x00A2;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Three-byte UTF-8 character") {
    const char *input = u8"€";
    const es::unicode_char expected = 0x20AC;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Four-byte UTF-8 character") {
    const char *input = u8"𐍈";
    const es::unicode_char expected = 0x10348;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Invalid UTF-8 character") {
    const char *input = "\xFF";
    const es::unicode_char expected = ~0;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Empty string") {
    const char *input = "";
    const es::unicode_char expected = 0;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Buffer underflow") {
    const char *input = "\xC2";
    const es::unicode_char expected = ~0;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Null pointer") {
    const char *input = nullptr;
    const es::unicode_char expected = ~0;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Longer section of text") {
    const char *input = u8"Я люблю колбасу";
    const es::unicode_char expected[] = {
        0x042F, 0x0020, 0x043B, 0x044E, 0x0431, 0x043B, 0x044E, 0x0020,
        0x043A, 0x043E, 0x043B, 0x0431, 0x0430, 0x0441, 0x0443, 0x00};
    for (auto &e : expected) {
      REQUIRE(es::next_utf8(input) == e);
    }
  }
}
