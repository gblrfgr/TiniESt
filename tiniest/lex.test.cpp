#include <catch2/catch_test_macros.hpp>
#include <tiniest/lex.hpp>

TEST_CASE("Character indicators") {
  SECTION("Whitespace") {
    REQUIRE(es::is_whitespace(' '));
    REQUIRE(es::is_whitespace('\t'));
    REQUIRE(es::is_whitespace('\v'));
    REQUIRE(es::is_whitespace('\f'));
    REQUIRE_FALSE(es::is_whitespace('\n'));
    REQUIRE_FALSE(es::is_whitespace('\r'));
    REQUIRE_FALSE(es::is_whitespace('a'));
  }
  SECTION("Line terminators") {
    REQUIRE(es::is_line_terminator('\n'));
    REQUIRE(es::is_line_terminator('\r'));
    REQUIRE_FALSE(es::is_line_terminator(' '));
    REQUIRE_FALSE(es::is_line_terminator('\t'));
    REQUIRE_FALSE(es::is_line_terminator('\v'));
    REQUIRE_FALSE(es::is_line_terminator('\f'));
  }
  SECTION("Identifier starts") {
    REQUIRE(es::is_identifier_start('$'));
    REQUIRE(es::is_identifier_start('_'));
    REQUIRE(es::is_identifier_start('a'));
    REQUIRE(es::is_identifier_start('z'));
    REQUIRE(es::is_identifier_start('A'));
    REQUIRE(es::is_identifier_start('Z'));
    REQUIRE_FALSE(es::is_identifier_start('0'));
    REQUIRE_FALSE(es::is_identifier_start('9'));
  }
}

TEST_CASE("Literals (happy path)") {
  SECTION("Hexadecimal") {
    const char *input = "0x1234567890abcdefABCDEF";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::NUMBER);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 24);
  }
  SECTION("Decimal") {
    const char *input = "1234567890";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::NUMBER);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 10);
  }
  SECTION("Floating point non-exponent") {
    const char *input = "123.456";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::NUMBER);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 7);
  }
  SECTION("Floating point exponent") {
    const char *input = "123.456e+789";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::NUMBER);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 12);
  }
  SECTION("Floating point exponent (uppercase)") {
    const char *input = "123.456E+789";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::NUMBER);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 12);
  }
  SECTION("Floating point exponent (negative)") {
    const char *input = "123.456e-789";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::NUMBER);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 12);
  }
  SECTION("Floating point exponent (negative uppercase)") {
    const char *input = "123.456E-789";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::NUMBER);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 12);
  }
  SECTION("Basic string") {
    const char *input = "\"Hello, world!\"";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::STRING);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 15);
  }
  SECTION("String with escape codes") {
    const char *input = "\"Hello,\\nworld!\"";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::STRING);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 16);
  }
  SECTION("String with line terminator") {
    const char *input = "\"Hello, \\\nworld!\"";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::STRING);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 17);
  }
}

TEST_CASE("Integration") {
  SECTION("Single statement") {
    const char *input = "var x = Serial.readLine();";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::VAR);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 3);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::IDENTIFIER);
    REQUIRE(token.start == input + 4);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::ASSIGNMENT);
    REQUIRE(token.start == input + 6);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::IDENTIFIER);
    REQUIRE(token.start == input + 8);
    REQUIRE(token.length == 6);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::DOT);
    REQUIRE(token.start == input + 14);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::IDENTIFIER);
    REQUIRE(token.start == input + 15);
    REQUIRE(token.length == 8);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::OPEN_PAREN);
    REQUIRE(token.start == input + 23);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::CLOSE_PAREN);
    REQUIRE(token.start == input + 24);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::SEMICOLON);
    REQUIRE(token.start == input + 25);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::END_OF_FILE);
    REQUIRE(token.start == input + 26);
    REQUIRE(token.length == 0);
  }

  SECTION("Simple function") {
    const char *input = R"(
      function add(a, b) {
        return a + b;
      }
    )";
    auto lexer = es::Lexer(input);
    auto token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::LINE_TERMINATOR);
    REQUIRE(token.start == input);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::FUNCTION);
    REQUIRE(token.start == input + 7);
    REQUIRE(token.length == 8);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::IDENTIFIER);
    REQUIRE(token.start == input + 16);
    REQUIRE(token.length == 3);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::OPEN_PAREN);
    REQUIRE(token.start == input + 19);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::IDENTIFIER);
    REQUIRE(token.start == input + 20);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::COMMA);
    REQUIRE(token.start == input + 21);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::IDENTIFIER);
    REQUIRE(token.start == input + 23);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::CLOSE_PAREN);
    REQUIRE(token.start == input + 24);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::OPEN_BRACE);
    REQUIRE(token.start == input + 26);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::LINE_TERMINATOR);
    REQUIRE(token.start == input + 27);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::RETURN);
    REQUIRE(token.start == input + 36);
    REQUIRE(token.length == 6);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::IDENTIFIER);
    REQUIRE(token.start == input + 43);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::PLUS);
    REQUIRE(token.start == input + 45);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::IDENTIFIER);
    REQUIRE(token.start == input + 47);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::SEMICOLON);
    REQUIRE(token.start == input + 48);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::LINE_TERMINATOR);
    REQUIRE(token.start == input + 49);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::CLOSE_BRACE);
    REQUIRE(token.start == input + 56);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::LINE_TERMINATOR);
    REQUIRE(token.start == input + 57);
    REQUIRE(token.length == 1);
    token = lexer.next_token();
    REQUIRE(token.type == es::Token::Type::END_OF_FILE);
    REQUIRE(token.start == input + 62);
    REQUIRE(token.length == 0);
  }
}