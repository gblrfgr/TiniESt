#ifndef TINIEST_LEX_HPP
#define TINIEST_LEX_HPP

#include <cstddef>
#include <cstdint>

namespace es {

using unicode_char = std::uint32_t; // unicode codepoint

/// @brief Get the next unicode character from a UTF-8 encoded string. Returns
/// ~0 if the string is nullptr or invalid.
/// @param cursor Reference to a pointer to the current position in the string.
/// @return The next unicode character.
[[nodiscard]] unicode_char next_utf8(const std::uint8_t *&cursor) noexcept;

/// @brief Put a unicode character into a UTF-8 encoded string.
/// @param chr The character to put.
/// @param cursor The current position in the string buffer. Will be updated if
/// there is sufficient space for the character.
/// @param space_remaining The remaining space in the string buffer. Will be
/// updated if there is sufficient space for the character.
void put_utf8(unicode_char chr, std::uint8_t *&cursor,
              std::size_t &space_remaining) noexcept;

/// @brief Determine if a unicode character is whitespace according to the
/// ECMAScript 5.1 specification. This includes spaces and tabs, but not line
/// separators (for the most part).
/// @param chr The unicode character to check.
/// @return A bool indicating whether the character is whitespace.
[[nodiscard]] bool is_whitespace(unicode_char chr) noexcept;

/// @brief Determine if a unicode character is a line terminator according to
/// the ECMAScript 5.1 specification.
/// @param chr The unicode character to check.
/// @return A bool indicating whether the character is a line terminator.
[[nodiscard]] bool is_line_terminator(unicode_char chr) noexcept;

/// @brief Represents a single ECMAScript token.
struct Token {
  enum class Type {
    IDENTIFIER_OR_KEYWORD,
    NUMBER,
    STRING,
    PUNCTUATOR,
    END_OF_FILE,
    ERROR
  };
  Type type;
  std::size_t line;
  std::size_t column;
  std::size_t length; // bytes
  const std::uint8_t *start;

  Token() = delete;
  Token(Type type, std::size_t line, std::size_t column, std::size_t length,
        const std::uint8_t *start)
      : type(type), line(line), column(column), length(length), start(start) {}
};

/// @brief Retrieves the next ECMAScript token from a UTF-8 encoded string.
/// Skips whitespace and comments.
/// @param buffer The string buffer to read from.
/// @return The next token.
[[nodiscard]] Token next_token(std::uint8_t *buffer) noexcept;

} // namespace es

#endif // TINIEST_LEX_HPP