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
#include <cstdlib>
#include <fstream>
#include <stack>
#include <unistd.h>

#include <peelo/unicode.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/quote.hpp"

static void repl(laskin::context&);
static void run_file(laskin::context&, std::istream&);

int main(int argc, char** argv)
{
  laskin::context context;

  if (argc > 1)
  {
    for (int i = 1; i < argc; ++i)
    {
      std::ifstream input(argv[i], std::ios_base::in);

      if (input.good())
      {
        run_file(context, input);
      } else {
        std::cerr << argv[0]
                  << ": Unable to open file `"
                  << argv[i]
                  << "' for reading."
                  << std::endl;
        std::exit(EXIT_FAILURE);
      }
    }
  }
  else if (isatty(fileno(stdin)))
  {
    repl(context);
  } else {
    run_file(context, std::cin);
  }

  return EXIT_SUCCESS;
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
          if (line[i] == '"')
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

static void repl(laskin::context& context)
{
  int line_counter = 0;
  std::u32string source;
  std::stack<char> open_braces;

  while (std::cin.good())
  {
    std::string line;

    std::cout << "laskin:"
              << ++line_counter
              << ':'
              << context.data().size()
              << (open_braces.empty() ? '>' : '*')
              << ' ';
    if (!std::getline(std::cin, line))
    {
      break;
    }
    else if (line.empty())
    {
      continue;
    }
    source.append(peelo::unicode::utf8::decode(line));
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

static void run_file(laskin::context& context, std::istream& input)
{
  const auto source = peelo::unicode::utf8::decode(
    std::string(
      std::istreambuf_iterator<char>(input),
      std::istreambuf_iterator<char>()
    )
  );

  try
  {
    laskin::quote::parse(source).call(context, std::cout);
  }
  catch (const laskin::error& error)
  {
    std::cerr << error << std::endl;
    std::exit(EXIT_FAILURE);
  }
}
