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
#include <fstream>
#include <unistd.h>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/quote.hpp"

static std::string programfile;
static std::vector<std::u32string> inline_scripts;

namespace laskin::cli
{
  void run_repl(context&);
}

static void run_file(laskin::context&, std::istream&);
static void parse_args(int, char**);
static void print_usage(std::ostream&, const char*);

int main(int argc, char** argv)
{
  laskin::context context;

  parse_args(argc, argv);

  if (!inline_scripts.empty())
  {
    for (const auto& source : inline_scripts)
    {
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
  }
  else if (!programfile.empty())
  {
    std::ifstream input(programfile, std::ios_base::in);

    if (!input.good())
    {
      std::cerr << argv[0]
                << ": Unable to open file `"
                << programfile
                << "' for reading."
                << std::endl;
      std::exit(EXIT_FAILURE);
    }
    run_file(context, input);
  }
  else if (isatty(fileno(stdin)))
  {
    laskin::cli::run_repl(context);
  } else {
    run_file(context, std::cin);
  }

  return EXIT_SUCCESS;
}

static void run_file(laskin::context& context, std::istream& input)
{
  const auto source = peelo::unicode::encoding::utf8::decode(
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

static void parse_args(int argc, char** argv)
{
  int offset = 1;

  while (offset < argc)
  {
    auto arg = argv[offset++];

    if (!*arg)
    {
      continue;
    }
    else if (*arg != '-')
    {
      programfile = arg;
      break;
    }
    else if (!arg[1])
    {
      break;
    }
    else if (arg[1] == '-')
    {
      if (!std::strcmp(arg, "--help"))
      {
        print_usage(std::cout, argv[0]);
        std::exit(EXIT_SUCCESS);
      }
      else if (!std::strcmp(arg, "--version"))
      {
        std::cerr << "Laskin 2.0.0" << std::endl;
        std::exit(EXIT_SUCCESS);
      } else {
        std::cerr << "Unrecognized switch: " << arg << std::endl;
        print_usage(std::cerr, argv[0]);
        std::exit(EXIT_FAILURE);
      }
    }
    for (int i = 1; arg[i]; ++i)
    {
      switch (arg[i])
      {
        case 'e':
          if (offset < argc)
          {
            std::u32string script;

            if (!peelo::unicode::encoding::utf8::decode_validate(
                  argv[offset++],
                  script
                ))
            {
              std::cerr << "Unable to decode given inline script as UTF-8."
                        << std::endl;
              std::exit(EXIT_FAILURE);
            }
            inline_scripts.push_back(script);
          } else {
            std::cerr << "Argument expected for the -e option." << std::endl;
            print_usage(std::cerr, argv[0]);
            std::exit(EXIT_FAILURE);
          }
          break;

        case 'h':
          print_usage(std::cout, argv[0]);
          std::exit(EXIT_SUCCESS);
          break;

        default:
          std::cerr << "Unrecognized switch: `" << arg[i] << "'" << std::endl;
          std::exit(EXIT_FAILURE);
          break;
      }
    }
  }

  if (offset < argc)
  {
    std::cerr << "Too many arguments given." << std::endl;
    print_usage(std::cerr, argv[0]);
    std::exit(EXIT_FAILURE);
  }
}

static void print_usage(std::ostream& output, const char* executable_name)
{
  output << std::endl
         << "Usage: "
         << executable_name
         << " [switches] [programfile]"
         << std::endl
         << "  -e program        One line of program. (Omit programfile.)"
         << std::endl
         << "  --version         Print the version."
         << std::endl
         << "  --help            Display this message."
         << std::endl
         << std::endl;
}
