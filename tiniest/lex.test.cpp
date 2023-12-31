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

TEST_CASE("is_whitespace behaves correctly") {
  SECTION("Basic types") {
    REQUIRE(es::is_whitespace(0x09));
    REQUIRE(es::is_whitespace(0x0B));
    REQUIRE(es::is_whitespace(0x0C));
    REQUIRE(es::is_whitespace(0x20));
    REQUIRE(es::is_whitespace(0xA0));
    REQUIRE(es::is_whitespace(0xFEFF));
  }

  SECTION("Various widths of spaces") {
    REQUIRE(es::is_whitespace(0x2000));
    REQUIRE(es::is_whitespace(0x2001));
    REQUIRE(es::is_whitespace(0x2002));
    REQUIRE(es::is_whitespace(0x2003));
    REQUIRE(es::is_whitespace(0x2004));
    REQUIRE(es::is_whitespace(0x2005));
    REQUIRE(es::is_whitespace(0x2006));
    REQUIRE(es::is_whitespace(0x2007));
    REQUIRE(es::is_whitespace(0x2008));
    REQUIRE(es::is_whitespace(0x2009));
    REQUIRE(es::is_whitespace(0x200A));
  }

  SECTION("Other unicode spaces") {
    REQUIRE(es::is_whitespace(0x1680));
    REQUIRE(es::is_whitespace(0x202F));
    REQUIRE(es::is_whitespace(0x205F));
    REQUIRE(es::is_whitespace(0x3000));
  }

  SECTION("Non-whitespace characters") {
    REQUIRE(!es::is_whitespace(0x00));
    REQUIRE(!es::is_whitespace(0x01));
    REQUIRE(!es::is_whitespace(0x02));
    REQUIRE(!es::is_whitespace(0x03));
    REQUIRE(!es::is_whitespace(0x04));
    REQUIRE(!es::is_whitespace(0x05));
    REQUIRE(!es::is_whitespace(0x06));
    REQUIRE(!es::is_whitespace(0x07));
    REQUIRE(!es::is_whitespace(0x08));
    REQUIRE(!es::is_whitespace(0x0A));
    REQUIRE(!es::is_whitespace(0x0D));
    REQUIRE(!es::is_whitespace(0x0E));
    REQUIRE(!es::is_whitespace(0x0F));
    REQUIRE(!es::is_whitespace(0x10));
    REQUIRE(!es::is_whitespace(0x11));
    REQUIRE(!es::is_whitespace(0x12));
    REQUIRE(!es::is_whitespace(0x13));
  }
}