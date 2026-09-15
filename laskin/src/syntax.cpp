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
#include <algorithm>
#include <vector>

#include <peelo/number.hpp>

#include "laskin/syntax.hpp"

namespace laskin::syntax
{
  static inline bool
  is_digit(char32_t c)
  {
    return c >= U'0' && c <= U'9';
  }

  static inline bool
  starts_with(
    const std::u32string& line,
    std::size_t pos,
    const char32_t* literal
  )
  {
    for (std::size_t i = 0; literal[i]; ++i)
    {
      if (pos + i >= line.length() || line[pos + i] != literal[i])
      {
        return false;
      }
    }

    return true;
  }

  static inline bool
  symbol_boundary_after(
    const std::u32string& line,
    std::size_t pos
  )
  {
    if (pos >= line.length())
    {
      return true;
    }

    return !is_symbol(line[pos]);
  }

  static const std::vector<std::u32string>&
  unit_symbols()
  {
    static const std::vector<std::u32string> symbols = []() {
      std::vector<std::u32string> result;

      for (const auto type : {
        peelo::number::unit::type::length,
        peelo::number::unit::type::mass,
        peelo::number::unit::type::time
      })
      {
        for (const auto& u : peelo::number::unit::all_units_of(type))
        {
          result.emplace_back(
            u.symbol.begin(),
            u.symbol.end()
          );
        }
      }

      std::sort(
        result.begin(),
        result.end(),
        [](const std::u32string& a, const std::u32string& b) {
          return a.size() > b.size();
        }
      );

      return result;
    }();

    return symbols;
  }

  static bool
  match_unit(
    const std::u32string& line,
    std::size_t pos,
    std::size_t& end
  )
  {
    for (const auto& unit_str : unit_symbols())
    {
      if (
        pos + unit_str.length() <= line.length()
        && line.compare(pos, unit_str.length(), unit_str) == 0
        && symbol_boundary_after(line, pos + unit_str.length())
      )
      {
        end = pos + unit_str.length();

        return true;
      }
    }

    return false;
  }

  static bool
  parse_number_body(
    const std::u32string& line,
    std::size_t pos,
    std::size_t& end
  )
  {
    if (pos >= line.length() || !is_digit(line[pos]))
    {
      return false;
    }

    end = pos;

    while (end < line.length() && is_digit(line[end]))
    {
      ++end;
    }

    while (
      end < line.length()
      && line[end] == U'_'
      && end + 1 < line.length()
      && is_digit(line[end + 1])
    )
    {
      ++end;

      while (end < line.length() && is_digit(line[end]))
      {
        ++end;
      }
    }

    if (
      end < line.length()
      && line[end] == U'.'
      && end + 1 < line.length()
      && is_digit(line[end + 1])
    )
    {
      ++end;

      while (end < line.length() && is_digit(line[end]))
      {
        ++end;
      }

      while (
        end < line.length()
        && line[end] == U'_'
        && end + 1 < line.length()
        && is_digit(line[end + 1])
      )
      {
        ++end;

        while (end < line.length() && is_digit(line[end]))
        {
          ++end;
        }
      }
    }

    std::size_t unit_end = end;

    if (match_unit(line, end, unit_end))
    {
      end = unit_end;
    }

    return end > pos;
  }

  static bool
  parse_number(
    const std::u32string& line,
    std::size_t pos,
    std::size_t& end
  )
  {
    if (pos >= line.length())
    {
      return false;
    }

    const auto prev = pos > 0 ? line[pos - 1] : 0;
    const bool after_dot = prev == U'.';

    if (after_dot)
    {
      return false;
    }

    std::size_t body_start = pos;
    std::size_t body_end = pos;

    if (line[pos] == U'+' || line[pos] == U'-')
    {
      const auto sign = line[pos];

      if (sign == U'-' && starts_with(line, pos, U"-inf"))
      {
        end = pos + 4;
        return symbol_boundary_after(line, end);
      }

      if (sign == U'+' && !parse_number_body(line, pos + 1, body_end))
      {
        return false;
      }

      if (sign == U'-' && !parse_number_body(line, pos + 1, body_end))
      {
        return false;
      }

      body_start = pos + 1;

      if (body_end > body_start)
      {
        end = body_end;

        return true;
      }

      return false;
    }

    if (parse_number_body(line, pos, body_end))
    {
      end = body_end;

      return true;
    }

    return false;
  }

  static std::size_t
  read_symbol(
    const std::u32string& line,
    std::size_t pos
  )
  {
    const auto start = pos;

    while (pos < line.length() && is_symbol(line[pos]))
    {
      ++pos;
    }

    return pos - start;
  }

  void
  highlight_source_line(
    const std::u32string& line,
    const highlight_callback& apply,
    const dictionary_predicate& is_dictionary_word
  )
  {
    if (!apply)
    {
      return;
    }

    std::size_t pos = 0;
    const auto length = line.length();

    while (pos < length)
    {
      const auto c = line[pos];

      if (c == U'#')
      {
        apply(pos, length - pos, highlight_kind::comment);
        break;
      }

      if (c == U'"' || c == U'\'')
      {
        const auto quote = c;
        const auto string_start = pos;

        ++pos;

        while (pos < length)
        {
          if (line[pos] == quote)
          {
            ++pos;
            break;
          }

          if (line[pos] == U'\\' && pos + 1 < length)
          {
            pos += 2;
          } else {
            ++pos;
          }
        }

        apply(string_start, pos - string_start, highlight_kind::string);
        continue;
      }

      std::size_t number_end = pos;

      if (parse_number(line, pos, number_end))
      {
        apply(pos, number_end - pos, highlight_kind::number);
        pos = number_end;
        continue;
      }

      if (is_separator(c))
      {
        apply(pos, 1, highlight_kind::delimiter);
        ++pos;
        continue;
      }

      if (is_symbol(c))
      {
        const auto symbol_len = read_symbol(line, pos);

        if (symbol_len > 0)
        {
          if (is_dictionary_word)
          {
            const std::u32string symbol = line.substr(pos, symbol_len);

            if (is_dictionary_word(symbol))
            {
              apply(pos, symbol_len, highlight_kind::symbol);
            }
          }

          pos += symbol_len;
          continue;
        }
      }

      ++pos;
    }
  }
}
