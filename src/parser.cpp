/*
 * Copyright (c) 2018, Rauli Laine
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
#include <peelo/unicode.hpp>

#include "laskin/error.hpp"
#include "laskin/quote.hpp"

namespace laskin
{
  static inline bool issymbol(char32_t);

  namespace
  {
    class parser
    {
    public:
      explicit parser(const std::u32string& source,
                      int line,
                      int column)
        : m_pos(std::begin(source))
        , m_end(std::end(source))
        , m_line(line)
        , m_column(column) {}

      parser(const parser&) = delete;
      parser(parser&&) = delete;
      void operator=(const parser&) = delete;
      void operator=(parser&&) = delete;

      /**
       * Parsers top level script into sequence of AST nodes.
       */
      std::vector<std::shared_ptr<node>> parse_script()
      {
        std::vector<std::shared_ptr<node>> nodes;

        for (;;)
        {
          skip_whitespace();
          if (eof())
          {
            break;
          }
          nodes.push_back(parse_statement());
        }

        return nodes;
      }

    private:
      std::shared_ptr<node> parse_statement()
      {
        skip_whitespace();

        if (eof())
        {
          throw error(
            error::type_syntax,
            U"Unexpected end of input; Missing statement.",
            m_line,
            m_column
          );
        }

        switch (peek())
        {
          case '(':
            return parse_quote_literal();

          case '[':
            return parse_vector_literal();

          default:
            return parse_statement_symbol();
        }
      }

      std::shared_ptr<node> parse_expression()
      {
        skip_whitespace();

        if (eof())
        {
          throw error(
            error::type_syntax,
            U"Unexpected end of input; Missing expression.",
            m_line,
            m_column
          );
        }

        switch (peek())
        {
          case '(':
            return parse_quote_literal();

          case '[':
            return parse_vector_literal();

          default:
            return parse_symbol();
        }
      }

      std::shared_ptr<node::literal> parse_quote_literal()
      {
        quote::node_container nodes;
        int line;
        int column;

        skip_whitespace();

        line = m_line;
        column = m_column;

        if (!peek_read('('))
        {
          throw error(
            error::type_syntax,
            std::u32string(U"Unexpected ") +
            (eof() ? U"end of input" : U"input") +
            U"; Missing quote literal.",
            line,
            column
          );
        }

        skip_whitespace();

        if (!peek_read(')'))
        {
          for (;;)
          {
            if (eof())
            {
              throw error(
                error::type_syntax,
                U"Unterminated quote literal; Missing `)'",
                line,
                column
              );
            }
            else if (peek_read(')'))
            {
              break;
            }
            nodes.push_back(parse_statement());
            skip_whitespace();
          }
        }

        return std::make_shared<node::literal>(
          value::make_quote(quote(nodes)),
          line,
          column
        );
      }

      std::shared_ptr<node::vector_literal> parse_vector_literal()
      {
        node::vector_literal::container_type elements;
        int line;
        int column;

        skip_whitespace();

        line = m_line;
        column = m_column;

        if (!peek_read('['))
        {
          throw error(
            error::type_syntax,
            std::u32string(U"Unexpected ") +
            (eof() ? U"end of input" : U"input") +
            U"; Missing vector literal.",
            line,
            column
          );
        }

        skip_whitespace();

        if (!peek_read(']'))
        {
          for (;;)
          {
            if (eof())
            {
              throw error(
                error::type_syntax,
                U"Unterminated vector literal; Missing `]'",
                line,
                column
              );
            }
            else if (peek_read(']'))
            {
              break;
            }
            elements.push_back(parse_expression());
            skip_whitespace();
            if (peek_read(','))
            {
              continue;
            }
            else if (peek_read(']'))
            {
              break;
            } else {
              throw error(
                error::type_syntax,
                U"Unterminated vector literal; Missing `]'",
                line,
                column
              );
            }
          }
        }

        return std::make_shared<node::vector_literal>(elements, line, column);
      }

      std::shared_ptr<node::symbol> parse_symbol()
      {
        std::u32string buffer;
        int line;
        int column;

        skip_whitespace();

        line = m_line;
        column = m_column;

        if (!peek(issymbol))
        {
          throw error(
            error::type_syntax,
            std::u32string(U"Unexpected ") +
            (eof() ? U"end of input" : U"input") +
            U"; Missing symbol.",
            line,
            column
          );
        }

        do
        {
          buffer.append(1, read());
        }
        while (peek(issymbol));

        if (!buffer.compare(U"->"))
        {
          throw error(
            error::type_syntax,
            U"Unexpected definition; Missing symbol.",
            line,
            column
          );
        }

        return std::make_shared<node::symbol>(buffer, line, column);
      }

      std::shared_ptr<node> parse_statement_symbol()
      {
        std::u32string buffer;
        int line;
        int column;

        skip_whitespace();

        line = m_line;
        column = m_column;

        if (!peek(issymbol))
        {
          throw error(
            error::type_syntax,
            std::u32string(U"Unexpected ") +
            (eof() ? U"end of input" : U"input") +
            U"; Missing symbol or definition.",
            line,
            column
          );
        }

        do
        {
          buffer.append(1, read());
        }
        while (peek(issymbol));

        if (!buffer.compare(U"->"))
        {
          const auto symbol = parse_symbol();

          return std::make_shared<node::definition>(
            symbol->id(),
            line,
            column
          );
        }

        return std::make_shared<node::symbol>(buffer, line, column);
      }

      /**
       * Returns true if there are no more characters to be read from the
       * source code.
       */
      inline bool eof() const
      {
        return m_pos >= m_end;
      }

      /**
       * Advances to next character in the source code discarding the current
       * one.
       */
      inline void advance()
      {
        read();
      }

      /**
       * Advances to next character in the source code and returns the current
       * one.
       */
      char32_t read()
      {
        const auto result = *m_pos++;

        if (result == '\n')
        {
          ++m_line;
          m_column = 1;
        } else {
          ++m_column;
        }

        return result;
      }

      /**
       * Returns next character from the source code without advancing any
       * further.
       */
      inline char32_t peek() const
      {
        return *m_pos;
      }

      /**
       * Returns true if the next character from the source code equals with
       * the one given as argument.
       */
      inline bool peek(char32_t expected) const
      {
        return !eof() && peek() == expected;
      }

      /**
       * Returns true if the next character from the source code matches with
       * given callback function.
       */
      inline bool peek(bool (*callback)(char32_t)) const
      {
        return !eof() && callback(peek());
      }

      /**
       * Returns true and advances to next character in the source code if
       * current one equals with the one given as argument.
       */
      bool peek_read(char32_t expected)
      {
        if (peek(expected))
        {
          advance();

          return true;
        }

        return false;
      }

      /**
       * Skips whitespace and comments from the source code.
       */
      void skip_whitespace()
      {
        while (!eof())
        {
          // Skip line comments.
          if (peek_read('#'))
          {
            while (!eof())
            {
              if (peek_read('\n') || peek_read('\r'))
              {
                break;
              } else {
                advance();
              }
            }
          }
          else if (!peek(peelo::unicode::isspace))
          {
            return;
          } else {
            advance();
          }
        }
      }

    private:
      std::u32string::const_iterator m_pos;
      const std::u32string::const_iterator m_end;
      int m_line;
      int m_column;
    };
  }

  quote quote::parse(const std::u32string& source, int line, int column)
  {
    return quote(parser(source, line, column).parse_script());
  }

  static inline bool issymbol(char32_t c)
  {
    return c != '['
      && c != ']'
      && c != '('
      && c != ')'
      && c != ','
      && peelo::unicode::isgraph(c);
  }
}
