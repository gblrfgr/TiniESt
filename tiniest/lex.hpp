#ifndef TINIEST_LEX_HPP
#define TINIEST_LEX_HPP

#include <cstdint>

namespace es {

using unicode_char = std::uint32_t; // unicode codepoint

/// @brief Get the next unicode character from a UTF-8 encoded string. Returns
/// ~0 if the string is nullptr or invalid.
/// @param cursor Reference to a pointer to the current position in the string.
/// @return The next unicode character.
[[nodiscard]] unicode_char next_utf8(const char *&cursor) noexcept;

/// @brief Determine if a unicode character is whitespace according to the
/// ECMAScript 5.1 specification. This includes spaces and tabs, but not line
/// separators (for the most part).
/// @param chr The unicode character to check.
/// @return A bool indicating whether the character is whitespace.
[[nodiscard]] bool is_whitespace(unicode_char chr) noexcept;

} // namespace es

#endif // TINIEST_LEX_HPP