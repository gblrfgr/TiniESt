#include <cstddef>
#include <cstring>
#include <optional>
#include <tiniest/lex.hpp>

bool es::is_whitespace(char chr) noexcept {
  return (chr == 0x09 || chr == 0x0B || chr == 0x0C || chr == 0x20);
}

bool es::is_line_terminator(char chr) noexcept {
  return (chr == 0x0A) || (chr == 0x0D);
}

bool es::is_identifier_start(char chr) noexcept {
  if (chr == '$' || chr == '_') {
    return true;
  } else if (chr >= 'a' && chr <= 'z') {
    return true;
  } else if (chr >= 'A' && chr <= 'Z') {
    return true;
  }
  return false;
}

void es::Lexer::do_line_comment() noexcept {
  while (true) {
    char chr = *m_cursor;
    if (chr == '\0') {
      return;
    } else if (es::is_line_terminator(chr)) {
      return;
    } else {
      m_cursor++;
    }
  }
}

std::optional<es::Token> es::Lexer::do_block_comment() noexcept {
  bool newline_encountered = false;
  char chr;
  char lookahead = *m_cursor;
  if (lookahead == '\0') {
    return es::Token(es::Token::Type::ERROR, 0, m_cursor);
  }
  while (true) {
    chr = lookahead;
    if (lookahead != '\0') {
      m_cursor++;
      lookahead = *m_cursor;
    }
    if (chr == '\0') {
      return es::Token(es::Token::Type::ERROR, 0, m_cursor);
    } else if (es::is_line_terminator(chr)) {
      newline_encountered = true;
      if (chr == 0x0D && lookahead == 0x0A) {
        // handle CR-LF
        m_cursor++;
        lookahead = *m_cursor;
      }
    } else if (chr == '*' && lookahead == '/') {
      if (newline_encountered) {
        return es::Token(es::Token::Type::LINE_TERMINATOR, 0, m_cursor);
      } else {
        return std::nullopt;
      }
    }
  }
}

es::Token es::Lexer::do_identifier_or_keyword() noexcept {
  const char *start = m_cursor;
  std::size_t length;
  while (true) {
    char chr = *m_cursor;
    if (chr == '\0') {
      length = m_cursor - start;
      break;
    } else if (es::is_identifier_start(chr) || (chr >= '0' && chr <= '9')) {
      m_cursor++;
    } else {
      length = m_cursor - start;
      break;
    }
  }
  for (auto keyword : KEYWORDS) {
    if (std::strncmp(start, keyword.keyword, length) == 0 &&
        std::strlen(keyword.keyword) == length) {
      return es::Token(keyword.type, length, start);
    }
  }
  return es::Token(es::Token::Type::IDENTIFIER, length, start);
}

es::Token es::Lexer::do_number() noexcept {
  const char *start = m_cursor;
  if (*m_cursor == '0' && (*(m_cursor + 1) == 'x' || *(m_cursor + 1) == 'X')) {
    return do_hexadecimal();
  } else if (*m_cursor == '0' && *(m_cursor + 1) >= '0' &&
             *(m_cursor + 1) <= '9') {
    // leading zeroes are not allowed
    return es::Token(es::Token::Type::ERROR, 1, m_cursor);
  }
  bool decimal_point_encountered = false;
  while (true) {
    if (*m_cursor >= '0' && *m_cursor <= '9') {
      m_cursor++;
    } else if (*m_cursor == '.' && !decimal_point_encountered) {
      m_cursor++;
      decimal_point_encountered = true;
    } else {
      break;
    }
  }
  if (*m_cursor == 'e' || *m_cursor == 'E') {
    // exponent indicator
    m_cursor++;
    if (*m_cursor == '+' || *m_cursor == '-') {
      m_cursor++;
    }
    bool has_exponent_part = false;
    while (true) {
      if (*m_cursor >= '0' && *m_cursor <= '9') {
        has_exponent_part = true;
        m_cursor++;
      } else {
        break;
      }
    }
    if (!has_exponent_part) {
      // exponent indicator without exponent part
      return es::Token(es::Token::Type::ERROR, 1, m_cursor);
    }
  }
  return es::Token(es::Token::Type::NUMBER, m_cursor - start, start);
}

es::Token es::Lexer::do_hexadecimal() noexcept {
  const char *start = m_cursor;
  std::size_t length;
  m_cursor += 2; // skip the 0x
  while (true) {
    if ((*m_cursor >= '0' && *m_cursor <= '9') ||
        (*m_cursor >= 'a' && *m_cursor <= 'f') ||
        (*m_cursor >= 'A' && *m_cursor <= 'F')) {
      m_cursor++;
    } else {
      length = m_cursor - start;
      break;
    }
  }
  if (length == 2) {
    // 0x is not a valid hexadecimal number
    return es::Token(es::Token::Type::ERROR, length, start);
  }
  return es::Token(es::Token::Type::NUMBER, length, start);
}

es::Token es::Lexer::do_string() noexcept {
  const char *start = m_cursor;
  char quote = *m_cursor;
  m_cursor++;
  while (true) {
    if (*m_cursor == '\0') {
      return es::Token(es::Token::Type::ERROR, 0, m_cursor);
    } else if (*m_cursor == quote) {
      m_cursor++;
      break;
    } else if (*m_cursor == '\\') {
      m_cursor++;
      if (*m_cursor == '\0') {
        return es::Token(es::Token::Type::ERROR, 0, m_cursor);
      } else if (*m_cursor == 'b' || *m_cursor == 't' || *m_cursor == 'n' ||
                 *m_cursor == 'v' || *m_cursor == 'f' || *m_cursor == 'r' ||
                 *m_cursor == '"' || *m_cursor == '\'' || *m_cursor == '\\' ||
                 *m_cursor == '0') {
        m_cursor++;
      } else if (*m_cursor == 'x') {
        m_cursor++;
        if (*m_cursor >= '0' && *m_cursor <= '9') {
          m_cursor++;
          if (*m_cursor >= '0' && *m_cursor <= '9') {
            m_cursor++;
          } else {
            return es::Token(es::Token::Type::ERROR, 0, m_cursor);
          }
        } else {
          return es::Token(es::Token::Type::ERROR, 0, m_cursor);
        }
      } else if (es::is_line_terminator(*m_cursor)) {
        if (*m_cursor == 0x0D && *(m_cursor + 1) == 0x0A) {
          // handle CR-LF
          m_cursor++;
        }
        m_cursor++;
      }
    } else if (es::is_line_terminator(*m_cursor)) {
      return es::Token(es::Token::Type::ERROR, 0, m_cursor);
    } else {
      m_cursor++;
    }
  }
  return es::Token(es::Token::Type::STRING, m_cursor - start, start);
}

es::Token es::Lexer::do_punctuator() noexcept {
  // i'm so sorry for this
  // there isn't a better way to do this afaik
  const char *start = m_cursor;
  es::Token res = es::Token(es::Token::Type::ERROR, 1,
                            m_cursor); // default value
  m_cursor++;
  switch (*(m_cursor - 1)) {
  case '{':
    res = es::Token(es::Token::Type::OPEN_BRACE, 1, start);
    break;
  case '}':
    res = es::Token(es::Token::Type::CLOSE_BRACE, 1, start);
    break;
  case '(':
    res = es::Token(es::Token::Type::OPEN_PAREN, 1, start);
    break;
  case ')':
    res = es::Token(es::Token::Type::CLOSE_PAREN, 1, start);
    break;
  case '[':
    res = es::Token(es::Token::Type::OPEN_BRACKET, 1, start);
    break;
  case ']':
    res = es::Token(es::Token::Type::CLOSE_BRACKET, 1, start);
    break;
  case '.':
    res = es::Token(es::Token::Type::DOT, 1, start);
    break;
  case ';':
    res = es::Token(es::Token::Type::SEMICOLON, 1, start);
    break;
  case ',':
    res = es::Token(es::Token::Type::COMMA, 1, start);
    break;
  case '<':
    if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::LESS_THAN_OR_EQUAL, 2, start);
    } else if (*m_cursor == '<') {
      m_cursor++;
      if (*m_cursor == '=') {
        m_cursor++;
        res = es::Token(es::Token::Type::LEFT_SHIFT_ASSIGNMENT, 3, start);
      } else {
        res = es::Token(es::Token::Type::LEFT_SHIFT, 2, start);
      }
    } else {
      res = es::Token(es::Token::Type::LESS_THAN, 1, start);
    }
    break;
  case '>':
    if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::GREATER_THAN_OR_EQUAL, 2, start);
    } else if (*m_cursor == '>') {
      m_cursor++;
      if (*m_cursor == '=') {
        m_cursor++;
        res = es::Token(es::Token::Type::RIGHT_SHIFT_ASSIGNMENT, 3, start);
      } else if (*m_cursor == '>') {
        m_cursor++;
        if (*m_cursor == '=') {
          m_cursor++;
          res = es::Token(es::Token::Type::UNSIGNED_RIGHT_SHIFT_ASSIGNMENT, 4,
                          start);
        } else {
          res = es::Token(es::Token::Type::UNSIGNED_RIGHT_SHIFT, 3, start);
        }
      } else {
        res = es::Token(es::Token::Type::RIGHT_SHIFT, 2, start);
      }
    } else {
      res = es::Token(es::Token::Type::GREATER_THAN, 1, start);
    }
    break;
  case '=':
    if (*m_cursor == '=') {
      m_cursor++;
      if (*m_cursor == '=') {
        m_cursor++;
        res = es::Token(es::Token::Type::STRICT_EQUAL, 3, start);
      } else {
        res = es::Token(es::Token::Type::EQUAL, 2, start);
      }
    } else {
      res = es::Token(es::Token::Type::ASSIGNMENT, 1, start);
    }
    break;
  case '!':
    if (*m_cursor == '=') {
      m_cursor++;
      if (*m_cursor == '=') {
        m_cursor++;
        res = es::Token(es::Token::Type::STRICT_NOT_EQUAL, 3, start);
      } else {
        res = es::Token(es::Token::Type::NOT_EQUAL, 2, start);
      }
    } else {
      res = es::Token(es::Token::Type::LOGICAL_NOT, 1, start);
    }
    break;
  case '+':
    if (*m_cursor == '+') {
      m_cursor++;
      res = es::Token(es::Token::Type::INCREMENT, 2, start);
    } else if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::PLUS_ASSIGNMENT, 2, start);
    } else {
      res = es::Token(es::Token::Type::PLUS, 1, start);
    }
    break;
  case '-':
    if (*m_cursor == '-') {
      m_cursor++;
      res = es::Token(es::Token::Type::DECREMENT, 2, start);
    } else if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::MINUS_ASSIGNMENT, 2, start);
    } else {
      res = es::Token(es::Token::Type::MINUS, 1, start);
    }
    break;
  case '*':
    if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::MULTIPLY_ASSIGNMENT, 2, start);
    } else {
      res = es::Token(es::Token::Type::MULTIPLY, 1, start);
    }
    break;
  case '/':
    if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::DIVIDE_ASSIGNMENT, 2, start);
    } else {
      res = es::Token(es::Token::Type::DIVIDE, 1, start);
    }
    break;
  case '%':
    if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::MODULO_ASSIGNMENT, 2, start);
    } else {
      res = es::Token(es::Token::Type::MODULO, 1, start);
    }
    break;
  case '&':
    if (*m_cursor == '&') {
      m_cursor++;
      res = es::Token(es::Token::Type::LOGICAL_AND, 2, start);
    } else if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::BITWISE_AND_ASSIGNMENT, 2, start);
    } else {
      res = es::Token(es::Token::Type::BITWISE_AND, 1, start);
    }
    break;
  case '|':
    if (*m_cursor == '|') {
      m_cursor++;
      res = es::Token(es::Token::Type::LOGICAL_OR, 2, start);
    } else if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::BITWISE_OR_ASSIGNMENT, 2, start);
    } else {
      res = es::Token(es::Token::Type::BITWISE_OR, 1, start);
    }
    break;
  case '^':
    if (*m_cursor == '=') {
      m_cursor++;
      res = es::Token(es::Token::Type::BITWISE_XOR_ASSIGNMENT, 2, start);
    } else {
      res = es::Token(es::Token::Type::BITWISE_XOR, 1, start);
    }
    break;
  case '~':
    res = es::Token(es::Token::Type::BITWISE_NOT, 1, start);
    break;
  case '?':
    res = es::Token(es::Token::Type::QUESTION_MARK, 1, start);
    break;
  case ':':
    res = es::Token(es::Token::Type::COLON, 1, start);
    break;
  default:
    break;
  }
  return res;
}

es::Token es::Lexer::next_token() noexcept {
  while (true) {
    if (*m_cursor == '\0') {
      return es::Token(es::Token::Type::END_OF_FILE, 0, m_cursor);
    } else if (es::is_line_terminator(*m_cursor)) {
      if (*m_cursor == 0x0D && *(m_cursor + 1) == 0x0A) {
        // skip the LF in CR-LF
        m_cursor += 2;
        return es::Token(es::Token::Type::LINE_TERMINATOR, 2, m_cursor - 2);
      } else {
        m_cursor++;
        return es::Token(es::Token::Type::LINE_TERMINATOR, 1, m_cursor - 1);
      }
    }
    if (*m_cursor == '/' && *(m_cursor + 1) == '/') {
      do_line_comment();
    } else if (*m_cursor == '/' && *(m_cursor + 1) == '*') {
      auto maybe_token = do_block_comment();
      if (maybe_token.has_value()) {
        return maybe_token.value();
      }
    } else if (*m_cursor == '\'' || *m_cursor == '"') {
      return do_string();
    } else if ((*m_cursor >= '0' && *m_cursor <= '9') ||
               (*m_cursor == '.' && *(m_cursor + 1) >= '0' &&
                *(m_cursor + 1) <= '9')) {
      return do_number();
    } else if (*m_cursor == '{' || *m_cursor == '}' || *m_cursor == '(' ||
               *m_cursor == ')' || *m_cursor == '[' || *m_cursor == ']' ||
               *m_cursor == '.' || *m_cursor == ';' || *m_cursor == ',' ||
               *m_cursor == '<' || *m_cursor == '>' || *m_cursor == '=' ||
               *m_cursor == '!' || *m_cursor == '+' || *m_cursor == '-' ||
               *m_cursor == '*' || *m_cursor == '%' || *m_cursor == '&' ||
               *m_cursor == '|' || *m_cursor == '^' || *m_cursor == '~' ||
               *m_cursor == '?' || *m_cursor == ':' || *m_cursor == '/') {
      return do_punctuator();
    } else if (es::is_whitespace(*m_cursor)) {
      m_cursor++;
    } else if (es::is_identifier_start(*m_cursor)) {
      return do_identifier_or_keyword();
    } else {
      return es::Token(es::Token::Type::ERROR, 1, m_cursor);
    }
  }
}
