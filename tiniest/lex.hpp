#ifndef TINIEST_LEX_HPP
#define TINIEST_LEX_HPP

#include <cstdint>

namespace es {

using unicode_char = std::uint32_t; // unicode codepoint

/// @brief Get the next unicode character from a UTF-8 encoded string.
/// @param cursor Reference to a pointer to the current position in the string.
/// @return The next unicode character.
unicode_char next_utf8(char *&cursor);

} // namespace es

#endif // TINIEST_LEX_HPP