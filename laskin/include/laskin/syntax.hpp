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
#pragma once

#include <cstddef>
#include <functional>
#include <stack>
#include <utility>

#include <peelo/unicode/ctype/isgraph.hpp>
#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/types.hpp"

namespace laskin::syntax
{
  /**
   * Tests whether given character is considered to be an separator character
   * in Laskin syntax.
   */
  template<class Char>
  inline bool
  is_separator(Char c)
  {
    return c == U'['
      || c == U']'
      || c == U'('
      || c == U')'
      || c == U'{'
      || c == U'}'
      || c == U','
      || c == U'"';
  }

  /**
   * Tests whether given character is considered to be part of a symbol in
   * Laskin syntax.
   */
  template<class Char>
  inline bool
  is_symbol(Char c)
  {
    return !is_separator(c) && peelo::unicode::ctype::isgraph(c);
  }

  /**
   * Updates a stack of unmatched opening delimiters for the given line.
   * Characters after an unquoted `#` comment marker are ignored.
   */
  template<class Char, class String>
  inline void
  count_open_braces(std::stack<Char>& braces, const String& line)
  {
    const auto length = line.length();

    for (std::size_t i = 0; i < length; ++i)
    {
      const Char c = static_cast<Char>(line[i]);

      switch (c)
      {
        case Char{'#'}:
          return;

        case Char{'('}:
          braces.push(Char{')'});
          break;

        case Char{'['}:
          braces.push(Char{']'});
          break;

        case Char{')'}:
        case Char{']'}:
          if (!braces.empty() && braces.top() == c)
          {
            braces.pop();
          }
          break;

        case Char{'"'}:
        case Char{'\''}:
          ++i;
          while (i < length)
          {
            const Char quoted = static_cast<Char>(line[i]);

            if (quoted == c)
            {
              break;
            }
            else if (
              quoted == Char{'\\'}
              && i + 1 < length
              && static_cast<Char>(line[i + 1]) == c
            )
            {
              i += 2;
            } else {
              ++i;
            }
          }
          break;
      }
    }
  }

  /**
   * Syntax highlighting kinds for Laskin source.
   */
  enum class highlight_kind
  {
    comment,
    string,
    number,
    delimiter,
    symbol,
  };

  inline constexpr std::size_t highlight_kind_count =
    static_cast<std::size_t>(highlight_kind::symbol) + 1;

  using highlight_callback = std::function<
    void(std::size_t start, std::size_t length, highlight_kind kind)
  >;

  using dictionary_predicate = std::function<bool(const std::u32string& word)>;

  /**
   * Applies syntax highlighting to a single line of Laskin source.
   * Spans are reported as codepoint indices into \a line.
   */
  void highlight_source_line(
    const std::u32string& line,
    const highlight_callback& apply,
    const dictionary_predicate& is_dictionary_word = nullptr
  );

  /**
   * Converts a codepoint range in UTF-8 encoded text to a byte offset and
   * length suitable for terminal highlighters such as Isocline.
   */
  inline std::pair<std::size_t, std::size_t>
  utf8_codepoint_range_to_bytes(
    const char* utf8,
    std::size_t utf8_length,
    std::size_t codepoint_start,
    std::size_t codepoint_count
  )
  {
    std::size_t byte_start = utf8_length;
    std::size_t byte_end = utf8_length;
    std::size_t cp = 0;
    std::size_t i = 0;

    while (i < utf8_length && cp < codepoint_start)
    {
      char32_t c = 0;

      if (
        !peelo::unicode::encoding::utf8::decode_advance(
          utf8,
          i,
          utf8_length,
          c
        )
      )
      {
        ++i;
      }

      ++cp;
    }

    byte_start = i;

    while (i < utf8_length && cp < codepoint_start + codepoint_count)
    {
      char32_t c = 0;

      if (
        !peelo::unicode::encoding::utf8::decode_advance(
          utf8,
          i,
          utf8_length,
          c
        )
      )
      {
        ++i;
      }

      ++cp;
    }

    byte_end = i;

    return { byte_start, byte_end - byte_start };
  }
}
