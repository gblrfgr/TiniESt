#include <catch2/catch_test_macros.hpp>
#include <tiniest/lex.hpp>

TEST_CASE("next_utf8 behaves correctly") {
  SECTION("Single-byte UTF-8 character") {
    const std::uint8_t *input = (std::uint8_t *) "A";
    const es::unicode_char expected = 'A';
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Two-byte UTF-8 character") {
    const std::uint8_t *input = (std::uint8_t *) u8"¢";
    const es::unicode_char expected = 0x00A2;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Three-byte UTF-8 character") {
    const std::uint8_t *input = (std::uint8_t *) u8"€";
    const es::unicode_char expected = 0x20AC;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Four-byte UTF-8 character") {
    const std::uint8_t *input = (std::uint8_t *) u8"𐍈";
    const es::unicode_char expected = 0x10348;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Invalid UTF-8 character") {
    const std::uint8_t *input = (std::uint8_t *) "\xFF";
    const es::unicode_char expected = ~0;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Empty string") {
    const std::uint8_t *input = (std::uint8_t *) "";
    const es::unicode_char expected = 0;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Buffer underflow") {
    const std::uint8_t *input = (std::uint8_t *) "\xC2";
    const es::unicode_char expected = ~0;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Null pointer") {
    const std::uint8_t *input = nullptr;
    const es::unicode_char expected = ~0;
    REQUIRE(es::next_utf8(input) == expected);
  }

  SECTION("Longer section of text") {
    const std::uint8_t *input = (std::uint8_t *) u8"Я люблю колбасу";
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

TEST_CASE("put_utf8 behaves correctly") {
  SECTION("Single-byte UTF-8 character") {
    std::uint8_t buffer[1];
    std::uint8_t* cursor = buffer;
    std::size_t space_remaining = 1;
    es::put_utf8('A', cursor, space_remaining);
    REQUIRE(buffer[0] == 'A');
    REQUIRE(cursor == buffer + 1);
    REQUIRE(space_remaining == 0);
  }

  SECTION("Two-byte UTF-8 character") {
    std::uint8_t buffer[2];
    std::uint8_t* cursor = buffer;
    std::size_t space_remaining = 2;
    es::put_utf8(0x00A2, cursor, space_remaining);
    REQUIRE(buffer[0] == 0xC2);
    REQUIRE(buffer[1] == 0xA2);
    REQUIRE(cursor == buffer + 2);
    REQUIRE(space_remaining == 0);
  }

  SECTION("Three-byte UTF-8 character") {
    std::uint8_t buffer[3];
    std::uint8_t* cursor = buffer;
    std::size_t space_remaining = 3;
    es::put_utf8(0x20AC, cursor, space_remaining);
    REQUIRE(buffer[0] == 0xE2);
    REQUIRE(buffer[1] == 0x82);
    REQUIRE(buffer[2] == 0xAC);
    REQUIRE(cursor == buffer + 3);
    REQUIRE(space_remaining == 0);
  }

  SECTION("Four-byte UTF-8 character") {
    std::uint8_t buffer[4];
    std::uint8_t* cursor = buffer;
    std::size_t space_remaining = 4;
    es::put_utf8(0x10348, cursor, space_remaining);
    REQUIRE(buffer[0] == 0xF0);
    REQUIRE(buffer[1] == 0x90);
    REQUIRE(buffer[2] == 0x8D);
    REQUIRE(buffer[3] == 0x88);
    REQUIRE(cursor == buffer + 4);
    REQUIRE(space_remaining == 0);
  }

  SECTION("Invalid UTF-8 character") {
    std::uint8_t buffer[1];
    std::uint8_t* cursor = buffer;
    std::size_t space_remaining = 1;
    es::put_utf8(0x110000, cursor, space_remaining);
    REQUIRE(cursor == buffer);
    REQUIRE(space_remaining == 1);
  }

  SECTION("Buffer overflow") {
    std::uint8_t buffer[1];
    std::uint8_t* cursor = buffer;
    std::size_t space_remaining = 1;
    es::put_utf8(0x20AC, cursor, space_remaining);
    REQUIRE(cursor == buffer);
    REQUIRE(space_remaining == 1);
  }
}

TEST_CASE("is_line_terminator behaves correctly") {
  SECTION("Basic types") {
    REQUIRE(es::is_line_terminator(0x0A));
    REQUIRE(es::is_line_terminator(0x0D));
  }

  SECTION("Other unicode line terminators") {
    REQUIRE(es::is_line_terminator(0x2028));
    REQUIRE(es::is_line_terminator(0x2029));
  }

  SECTION("Non-line-terminator characters") {
    REQUIRE(!es::is_line_terminator(0x00));
    REQUIRE(!es::is_line_terminator(0x01));
    REQUIRE(!es::is_line_terminator(0x02));
    REQUIRE(!es::is_line_terminator(0x03));
    REQUIRE(!es::is_line_terminator(0x04));
    REQUIRE(!es::is_line_terminator(0x05));
    REQUIRE(!es::is_line_terminator(0x06));
    REQUIRE(!es::is_line_terminator(0x07));
    REQUIRE(!es::is_line_terminator(0x08));
    REQUIRE(!es::is_line_terminator(0x09));
    REQUIRE(!es::is_line_terminator(0x0B));
    REQUIRE(!es::is_line_terminator(0x0C));
    REQUIRE(!es::is_line_terminator(0x0E));
    REQUIRE(!es::is_line_terminator(0x0F));
    REQUIRE(!es::is_line_terminator(0x10));
    REQUIRE(!es::is_line_terminator(0x11));
    REQUIRE(!es::is_line_terminator(0x12));
    REQUIRE(!es::is_line_terminator(0x13));
  }
}