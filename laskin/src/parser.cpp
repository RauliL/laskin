/*
 * Copyright (c) 2018-2026, Rauli Laine
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <peelo/unicode/ctype/isgraph.hpp>
#include <peelo/unicode/ctype/isspace.hpp>
#include <peelo/unicode/ctype/isvalid.hpp>
#include <peelo/unicode/ctype/isxdigit.hpp>

#include "laskin/error.hpp"
#include "laskin/quote.hpp"

namespace laskin
{
  struct state
  {
    struct position position;
    std::u32string::const_iterator pos;
    std::u32string::const_iterator end;
  };

  static std::shared_ptr<node> parse(struct state&, bool);

  static inline bool
  issymbol(char32_t c)
  {
    return c != U'['
      && c != U']'
      && c != U'('
      && c != U')'
      && c != U'{'
      && c != U'}'
      && c != U','
      && peelo::unicode::ctype::isgraph(c);
  }

  /**
   * Returns true if there are no more characters to be read from the source
   * code.
   */
  inline bool
  eof(const struct state& state)
  {
    return state.pos >= state.end;
  }

  /**
   * Advances to next character in the source code and returns the current one.
   */
  static char32_t
  read(struct state& state)
  {
    const auto result = *state.pos++;

    if (result == '\n')
    {
      ++state.position.line;
      state.position.column = 1;
    } else {
      ++state.position.column;
    }

    return result;
  }

  /**
   * Returns next character from the source code without advancing any further.
   */
  static inline char32_t
  peek(struct state& state)
  {
    return *state.pos;
  }

  /**
   * Returns `true` if the next character from the source code matches with the
   * one given as argument.
   */
  static inline bool
  peek(struct state& state, char32_t expected)
  {
    return !eof(state) && peek(state) == expected;
  }

  /**
   * Returns `true` if the next character from the source code matches with the
   * given callback function.
   */
  static inline bool
  peek(struct state& state, bool (*callback)(char32_t))
  {
    return !eof(state) && callback(peek(state));
  }

  /**
   * Returns `true` and advances to next character in source code if current
   * one equals with one given as argument.
   */
  static inline bool
  peek_read(struct state& state, char32_t expected)
  {
    if (peek(state, expected))
    {
      read(state);

      return true;
    }

    return false;
  }

  /**
   * Skips whitespace and comments from the source code.
   */
  static void
  skip_whitespace(struct state& state)
  {
    using peelo::unicode::ctype::isspace;

    while (!eof(state))
    {
      // Skip line comments.
      if (peek_read(state, U'#'))
      {
        while (!eof(state))
        {
          if (peek_read(state, U'\n') || peek_read(state, U'\r'))
          {
            break;
          } else {
            read(state);
          }
        }
      }
      else if (!peek(state, isspace))
      {
        return;
      } else {
        read(state);
      }
    }
  }

  static void
  parse_escape_sequence(struct state& state, std::u32string& buffer)
  {
    const auto position = state.position;

    if (eof(state))
    {
      throw error(
        error::type::syntax,
        U"Unexpected end of input; Missing escape sequence.",
        position
      );
    }

    switch (read(state))
    {
      case U'b':
        buffer.push_back(010);
        break;

      case U't':
        buffer.push_back(011);
        break;

      case U'n':
        buffer.push_back(012);
        break;

      case U'f':
        buffer.push_back(014);
        break;

      case U'r':
        buffer.push_back(015);
        break;

      case U'"':
      case U'\'':
      case U'\\':
      case U'/':
        buffer.push_back(*(state.pos - 1));
        break;

      case U'u':
      {
        char32_t result = 0;

        for (int i = 0; i < 4; ++i)
        {
          if (eof(state))
          {
            throw error(
              error::type::syntax,
              U"Unterminated escape sequence.",
              position
            );
          }
          else if (!peelo::unicode::ctype::isxdigit(peek(state)))
          {
            throw error(
              error::type::syntax,
              U"Illegal Unicode hex escape sequence.",
              position
            );
          }

          if (peek(state) >= 'A' && peek(state) <= 'F')
          {
            result = result * 16 + (read(state) - 'A' + 10);
          }
          else if (peek(state) >= 'a' && peek(state) <= 'f')
          {
            result = result * 16 + (read(state) - 'a' + 10);
          } else {
            result = result * 16 + (read(state) - '0');
          }
        }

        if (!peelo::unicode::ctype::isvalid(result))
        {
          throw error(
            error::type::syntax,
            U"Illegal Unicode hex escape sequence.",
            position
          );
        }

        buffer.push_back(result);
        break;
      }

      default:
        throw error(
          error::type::syntax,
          U"Illegal escape sequence in string literal.",
          position
        );
    }
  }

  static std::u32string
  parse_string(struct state& state)
  {
    struct position position;
    std::u32string buffer;
    char32_t separator;

    skip_whitespace(state);
    position = state.position;

    if (peek_read(state, U'"'))
    {
      separator = U'"';
    }
    else if (peek_read(state, U'\''))
    {
      separator = U'\'';
    } else {
      throw error(
        error::type::syntax,
        std::u32string(U"Unexpected ") +
        (eof(state) ? U"end of input" : U"input") +
        U"; Missing string literal.",
        position
      );
    }

    for (;;)
    {
      if (eof(state))
      {
        throw error(
          error::type::syntax,
          std::u32string(U"Unterminated string literal; Missing `") +
          separator +
          U"'.",
          position
        );
      }
      else if (peek_read(state, separator))
      {
        break;
      }
      else if (peek_read(state, U'\\'))
      {
        parse_escape_sequence(state, buffer);
      } else {
        buffer.push_back(read(state));
      }
    }

    return buffer;
  }

  static inline std::shared_ptr<node::literal>
  parse_string_literal(struct state& state)
  {
    const auto position = state.position;
    const auto value = parse_string(state);

    return std::make_shared<node::literal>(
      value::make_string(value),
      position
    );
  }

  static std::shared_ptr<node::vector_literal>
  parse_vector_literal(struct state& state)
  {
    node::vector_literal::container_type elements;
    struct position position;

    skip_whitespace(state);
    position = state.position;

    if (!peek_read(state, U'['))
    {
      throw error(
        error::type::syntax,
        std::u32string(U"Unexpected ") +
        (eof(state) ? U"end of input" : U"input") +
        U"; Missing vector literal.",
        position
      );
    }

    skip_whitespace(state);

    if (!peek_read(state, U']'))
    {
      for (;;)
      {
        if (eof(state))
        {
          throw error(
            error::type::syntax,
            U"Unterminated vector literal; Missing `]'.",
            position
          );
        }
        else if (peek_read(state, U']'))
        {
          break;
        }
        elements.push_back(parse(state, false));
        skip_whitespace(state);
        if (peek_read(state, U','))
        {
          continue;
        }
        else if (peek_read(state, U']'))
        {
          break;
        }

        throw error(
          error::type::syntax,
          U"Unterminated vector literal; Missing `]'.",
          position
        );
      }
    }

    return std::make_shared<node::vector_literal>(elements, position);
  }

  static std::shared_ptr<node::record_literal>
  parse_record_literal(struct state& state)
  {
    node::record_literal::container_type properties;
    struct position position;

    skip_whitespace(state);
    position = state.position;

    if (!peek_read(state, U'{'))
    {
      throw error(
        error::type::syntax,
        std::u32string(U"Unexpected ") +
        (eof(state) ? U"end of input" : U"input") +
        U"; Missing record literal.",
        position
      );
    }

    skip_whitespace(state);

    if (!peek_read(state, U'}'))
    {
      for (;;)
      {
        if (eof(state))
        {
          throw error(
            error::type::syntax,
            U"Unterminated record literal; Missing `}'.",
            position
          );
        }
        else if (peek_read(state, U'}'))
        {
          break;
        } else {
          const auto key = parse_string(state);
          std::shared_ptr<node> value;

          skip_whitespace(state);
          if (!peek_read(state, U':'))
          {
            throw error(
              error::type::syntax,
              U"Missing `:' after key.",
              state.position
            );
          }
          skip_whitespace(state);
          value = parse(state, false);
          properties[key] = value;
          skip_whitespace(state);
          if (peek_read(state, U','))
          {
            continue;
          }
          else if (peek_read(state, U'}'))
          {
            break;
          }

          throw error(
            error::type::syntax,
            U"Unterminated record literal; Missing `}'.",
            position
          );
        }
      }
    }

    return std::make_shared<node::record_literal>(properties, position);
  }

  static std::shared_ptr<node::literal>
  parse_quote_literal(struct state& state)
  {
    struct position position;

    quote::node_container nodes;

    skip_whitespace(state);
    position = state.position;

    if (!peek_read(state, U'('))
    {
      throw error(
        error::type::syntax,
        std::u32string(U"Unexpected ") +
        (eof(state) ? U"end of input" : U"input") +
        U"; Missing quote literal.",
        position
      );
    }

    skip_whitespace(state);

    if (!peek_read(state, U')'))
    {
      for (;;)
      {
        if (eof(state))
        {
          throw error(
            error::type::syntax,
            U"Unterminated quote literal; Missing `)'.",
            position
          );
        }
        else if (peek_read(state, U')'))
        {
          break;
        }
        nodes.push_back(parse(state, true));
        skip_whitespace(state);
      }
    }

    return std::make_shared<node::literal>(value::make_quote(nodes), position);
  }

  static std::u32string
  parse_symbol_string(struct state& state)
  {
    std::u32string buffer;

    skip_whitespace(state);
    if (!peek(state, issymbol))
    {
      throw error(
        error::type::syntax,
        std::u32string(U"Unexpected ") +
        (eof(state) ? U"end of input" : U"input") +
        U"; Missing symbol or definition.",
        state.position
      );
    }
    do
    {
      buffer.push_back(read(state));
    }
    while (peek(state, issymbol));

    return buffer;
  }

  static std::shared_ptr<node>
  parse_symbol(struct state& state, bool allow_definition)
  {
    std::u32string id;
    struct position position;

    skip_whitespace(state);
    position = state.position;
    if ((id = parse_symbol_string(state)) == U"->")
    {
      const auto symbol = parse_symbol_string(state);

      if (!allow_definition)
      {
        throw error(
          error::type::syntax,
          U"Definitions are not allowed in this context.",
          position
        );
      }

      return std::make_shared<node::definition>(symbol, position);
    }

    return std::make_shared<node::symbol>(id, position);
  }

  static std::shared_ptr<node>
  parse(struct state& state, bool allow_definition)
  {
    skip_whitespace(state);

    if (eof(state))
    {
      throw error(
        error::type::syntax,
        U"Unexpected end of input; Missing statement.",
        state.position
      );
    }

    switch (peek(state))
    {
      case '(':
        return parse_quote_literal(state);

      case '[':
        return parse_vector_literal(state);

      case '{':
        return parse_record_literal(state);

      case '"':
      case '\'':
        return parse_string_literal(state);

      default:
        return parse_symbol(state, allow_definition);
    }
  }

  quote
  quote::parse(
    const std::u32string& source,
    const std::optional<std::filesystem::path>& path,
    int line,
    int column
  )
  {
    struct state state =
    {
      path,
      line,
      column,
      std::begin(source),
      std::end(source),
    };
    quote::node_container nodes;

    for (;;)
    {
      skip_whitespace(state);
      if (eof(state))
      {
        break;
      }
      nodes.push_back(laskin::parse(state, true));
    }

    return quote(nodes);
  }
}
