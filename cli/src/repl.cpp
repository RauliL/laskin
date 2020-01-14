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
#include <cstdio>
#include <cstring>
#include <stack>

#include <peelo/prompt.hpp>
#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/quote.hpp"
#include "laskin/utils.hpp"

#if !defined(BUFSIZ)
# define BUFSIZ 1024
#endif

namespace laskin::cli
{
  static int line_counter = 0;
  static std::stack<char> open_braces;

  static const char* get_prompt(context&);
  static void count_open_braces(std::stack<char>&, const std::string&);

  void run_repl(class context& context)
  {
    peelo::prompt prompt;
    std::u32string source;

    while (auto optional_line = prompt.input(get_prompt(context)))
    {
      const auto& line = optional_line.value();

      if (utils::is_blank(line))
      {
        continue;
      }
      prompt.add_to_history(line);
      source.append(peelo::unicode::encoding::utf8::decode(line));
      source.append(1, '\n');
      count_open_braces(open_braces, line);
      if (!open_braces.empty())
      {
        continue;
      }
      try
      {
        laskin::quote::parse(
          source,
          line_counter
        ).call(context, std::cout);
      }
      catch (const laskin::error& error)
      {
        std::cout << error << std::endl;
      }
      source.clear();
    }
  }

  static const char* get_prompt(class context& context)
  {
    static char buffer[BUFSIZ];

    std::snprintf(
      buffer,
      BUFSIZ,
      "laskin:%03d:%ld%c ",
      ++line_counter,
      context.data().size(),
      open_braces.empty() ? '>' : '*'
    );

    return buffer;
  }

  static void count_open_braces(std::stack<char>& open_braces,
                                const std::string& line)
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
}
