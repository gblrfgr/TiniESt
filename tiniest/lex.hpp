#ifndef TINIEST_LEX_HPP
#define TINIEST_LEX_HPP

#include <cstddef>
#include <cstdint>
#include <optional>

namespace es {

[[nodiscard]] bool is_whitespace(char chr) noexcept;

[[nodiscard]] bool is_line_terminator(char chr) noexcept;

[[nodiscard]] bool is_identifier_start(char chr) noexcept;

/// @brief Represents a single ECMAScript token.
struct Token {
  enum class Type {
    IDENTIFIER,
    BREAK,
    DO,
    INSTANCEOF,
    TYPEOF,
    CASE,
    ELSE,
    NEW,
    VAR,
    CATCH,
    FINALLY,
    RETURN,
    VOID,
    CONTINUE,
    FOR,
    SWITCH,
    WHILE,
    DEBUGGER,
    FUNCTION,
    THIS,
    WITH,
    DEFAULT,
    IF,
    THROW,
    DELETE,
    IN,
    TRY,
    TRUE,
    FALSE,
    NULL_,
    NUMBER,
    STRING,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    DOT,
    SEMICOLON,
    COMMA,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN_OR_EQUAL,
    EQUAL,
    NOT_EQUAL,
    STRICT_EQUAL,
    STRICT_NOT_EQUAL,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    INCREMENT,
    DECREMENT,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    UNSIGNED_RIGHT_SHIFT,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    BITWISE_NOT,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,
    QUESTION_MARK,
    COLON,
    ASSIGNMENT,
    PLUS_ASSIGNMENT,
    MINUS_ASSIGNMENT,
    MULTIPLY_ASSIGNMENT,
    DIVIDE_ASSIGNMENT,
    MODULO_ASSIGNMENT,
    LEFT_SHIFT_ASSIGNMENT,
    RIGHT_SHIFT_ASSIGNMENT,
    UNSIGNED_RIGHT_SHIFT_ASSIGNMENT,
    BITWISE_AND_ASSIGNMENT,
    BITWISE_OR_ASSIGNMENT,
    BITWISE_XOR_ASSIGNMENT,
    END_OF_FILE,
    LINE_TERMINATOR,
    ERROR
  };
  Type type;
  std::size_t length; // bytes
  const char *start;

  Token() = delete;
  Token(Type type, std::size_t length, const char *start)
      : type(type), length(length), start(start) {}
};

struct Keyword {
  const char *keyword;
  Token::Type type;

  Keyword(const char *keyword, Token::Type type)
      : keyword(keyword), type(type) {}
};

const Keyword KEYWORDS[] = {
    Keyword("break", Token::Type::BREAK),
    Keyword("do", Token::Type::DO),
    Keyword("instanceof", Token::Type::INSTANCEOF),
    Keyword("typeof", Token::Type::TYPEOF),
    Keyword("case", Token::Type::CASE),
    Keyword("else", Token::Type::ELSE),
    Keyword("new", Token::Type::NEW),
    Keyword("var", Token::Type::VAR),
    Keyword("catch", Token::Type::CATCH),
    Keyword("finally", Token::Type::FINALLY),
    Keyword("return", Token::Type::RETURN),
    Keyword("void", Token::Type::VOID),
    Keyword("continue", Token::Type::CONTINUE),
    Keyword("for", Token::Type::FOR),
    Keyword("switch", Token::Type::SWITCH),
    Keyword("while", Token::Type::WHILE),
    Keyword("debugger", Token::Type::DEBUGGER),
    Keyword("function", Token::Type::FUNCTION),
    Keyword("this", Token::Type::THIS),
    Keyword("with", Token::Type::WITH),
    Keyword("default", Token::Type::DEFAULT),
    Keyword("if", Token::Type::IF),
    Keyword("throw", Token::Type::THROW),
    Keyword("delete", Token::Type::DELETE),
    Keyword("in", Token::Type::IN),
    Keyword("try", Token::Type::TRY),
    Keyword("true", Token::Type::TRUE),
    Keyword("false", Token::Type::FALSE),
    Keyword("null", Token::Type::NULL_),
};

/// @brief Separates an ASCII encoded string into ECMAScript tokens.
class Lexer {
private:
  const char *m_buffer_start;
  const char *m_cursor;

  void do_line_comment() noexcept;
  std::optional<Token> do_block_comment() noexcept;
  Token do_identifier_or_keyword() noexcept;
  Token do_number() noexcept;
  Token do_hexadecimal() noexcept;
  Token do_string() noexcept;
  Token do_punctuator() noexcept;

public:
  Lexer(const char *buffer) : m_buffer_start(buffer), m_cursor(buffer) {}
  /// @brief Retrieves the next ECMAScript token from the string. Skips
  /// whitespace and comments.
  /// @return The next token.
  [[nodiscard]] Token next_token() noexcept;
};

} // namespace es

#endif // TINIEST_LEX_HPP