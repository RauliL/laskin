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
#include <cstdio>
#include <cstring>
#include <stack>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/utils.hpp"

#include "./linenoise.hpp"

#if !defined(BUFSIZ)
# define BUFSIZ 1024
#endif

namespace laskin::cli
{
  static int line_counter = 0;
  static std::stack<char> open_braces;

  static const char* get_prompt(context&);
  static void count_open_braces(std::stack<char>&, const std::string&);
  static bool cursor_outside_string_or_comment(
    const char* line,
    std::size_t length,
    std::size_t pos
  );
  static bool find_symbol_at_cursor(
    const char* line,
    std::size_t length,
    std::size_t pos,
    std::size_t& word_start,
    std::size_t& word_end,
    std::u32string& prefix
  );
  static void complete_dictionary(
    const context& context,
    const char* line,
    std::size_t pos,
    std::vector<linenoise::Completion>& out
  );

  void
  run_repl(class context& context)
  {
    std::string source;

    linenoise::SetCompletionCallback(
      [&context](
        const char* line,
        std::size_t pos,
        std::vector<linenoise::Completion>& out
      )
      {
        complete_dictionary(context, line, pos, out);
      }
    );

    for (;;)
    {
      std::string line;
      const auto quit = linenoise::Readline(get_prompt(context), line);

      if (quit)
      {
        break;
      }
      linenoise::AddHistory(line.c_str());
      source.append(line).append(1, '\n');
      count_open_braces(open_braces, line);
      if (!open_braces.empty())
      {
        continue;
      }
      try
      {
        context.run(source, &std::cout, "<repl>", line_counter);
      }
      catch (const error& e)
      {
        if (e.is(laskin::error::type::exit))
        {
          std::exit(EXIT_FAILURE);
        } else {
          std::cout << e << std::endl;
        }
      }
      source.clear();
    }
  }

  static const char*
  get_prompt(class context& context)
  {
    static char buffer[BUFSIZ];

    std::snprintf(
      buffer,
      BUFSIZ,
#if defined(_WIN32)
      "laskin:%03d:%llu%c ",
#else
      "laskin:%03d:%lu%c ",
#endif
      ++line_counter,
      context.data.size(),
      open_braces.empty() ? '>' : '*'
    );

    return buffer;
  }

  static void
  count_open_braces(
    std::stack<char>& open_braces,
    const std::string& line
  )
  {
    const auto length = line.length();

    for (std::string::size_type i = 0; i < length; ++i)
    {
      const auto c = line[i];

      switch (line[i])
      {
        case '#':
          return;

        case '(':
          open_braces.push(')');
          break;

        case '[':
          open_braces.push(']');
          break;

        case ')':
        case ']':
          if (!open_braces.empty() && open_braces.top() == c)
          {
            open_braces.pop();
          }
          break;

        case '"':
        case '\'':
          ++i;
          while (i < length)
          {
            if (line[i] == c)
            {
              break;
            }
            else if (line[i] == '\\' && i + 1 < length && line[i + 1] == c)
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

  static bool
  cursor_outside_string_or_comment(
    const char* line,
    const std::size_t length,
    const std::size_t pos
  )
  {
    bool in_string = false;
    char quote = 0;

    for (std::size_t i = 0; i < pos; ++i)
    {
      if (!in_string && line[i] == '#')
      {
        return false;
      }
      if (!in_string && (line[i] == '"' || line[i] == '\''))
      {
        in_string = true;
        quote = line[i];
        continue;
      }
      if (in_string)
      {
        if (line[i] == '\\' && i + 1 < pos)
        {
          ++i;
          continue;
        }
        if (line[i] == quote)
        {
          in_string = false;
        }
      }
    }

    return !in_string;
  }

  static bool
  find_symbol_at_cursor(
    const char* line,
    const std::size_t length,
    std::size_t pos,
    std::size_t& word_start,
    std::size_t& word_end,
    std::u32string& prefix
  )
  {
    using peelo::unicode::encoding::utf8::codepoint_length;
    using peelo::unicode::encoding::utf8::decode_advance;

    struct codepoint
    {
      std::size_t byte_start;
      char32_t c;
    };

    std::vector<codepoint> codepoints;

    for (std::size_t i = 0; i < length;)
    {
      const auto byte_start = i;
      char32_t c = 0;

      if (!decode_advance(line, i, length, c))
      {
        ++i;
        continue;
      }

      codepoints.push_back({ byte_start, c });
    }

    if (codepoints.empty())
    {
      return false;
    }

    if (pos > length)
    {
      pos = length;
    }

    int cursor_index = -1;

    for (std::size_t i = 0; i < codepoints.size(); ++i)
    {
      if (codepoints[i].byte_start < pos)
      {
        cursor_index = static_cast<int>(i);
      } else {
        break;
      }
    }

    if (cursor_index < 0)
    {
      if (pos == 0 && utils::is_symbol(codepoints.front().c))
      {
        cursor_index = 0;
      } else {
        return false;
      }
    }

    if (!utils::is_symbol(codepoints[cursor_index].c))
    {
      if (
        static_cast<std::size_t>(cursor_index + 1) < codepoints.size()
        && codepoints[cursor_index + 1].byte_start == pos
        && utils::is_symbol(codepoints[cursor_index + 1].c)
      )
      {
        cursor_index += 1;
      } else {
        return false;
      }
    }

    int start_index = cursor_index;

    while (start_index > 0 && utils::is_symbol(codepoints[start_index - 1].c))
    {
      --start_index;
    }

    int end_index = cursor_index;

    while (
      static_cast<std::size_t>(end_index + 1) < codepoints.size()
      && utils::is_symbol(codepoints[end_index + 1].c)
    )
    {
      ++end_index;
    }

    word_start = codepoints[start_index].byte_start;
    word_end = codepoints[end_index].byte_start
      + codepoint_length(codepoints[end_index].c);

    prefix.clear();

    for (int i = start_index; i <= cursor_index; ++i)
    {
      prefix.push_back(codepoints[i].c);
    }

    return true;
  }

  static void
  complete_dictionary(
    const context& context,
    const char* line,
    const std::size_t pos,
    std::vector<linenoise::Completion>& out
  )
  {
    using peelo::unicode::encoding::utf8::encode;

    const auto length = std::strlen(line);
    std::size_t word_start = 0;
    std::size_t word_end = 0;
    std::u32string prefix;

    if (
      !cursor_outside_string_or_comment(line, length, pos)
      || !find_symbol_at_cursor(line, length, pos, word_start, word_end, prefix)
    )
    {
      return;
    }

    for (const auto& entry : context.dictionary)
    {
      if (entry.first.compare(0, prefix.size(), prefix) == 0)
      {
        out.push_back(
          {
            encode(entry.first),
            word_start,
            word_end
          }
        );
      }
    }
  }
}
