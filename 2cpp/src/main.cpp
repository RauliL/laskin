/*
 * Copyright (c) 2026, Rauli Laine
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
#include <cstring>
#include <fstream>

#include <laskin/error.hpp>
#include <laskin/macros.hpp>

#include "./program.hpp"

static std::filesystem::path input_path;
static std::optional<std::filesystem::path> output_path;
static bool output_path_set = false;

static void parse_args(int, char**);
static void print_usage(std::ostream&, const char*);

laskin2cpp::options options = { true, true };

int
main(int argc, char** argv)
{
  std::ifstream input;
  laskin2cpp::program program;
  laskin2cpp::writer writer;

  parse_args(argc, argv);

  input.open(input_path);
  if (!input.good())
  {
    std::cerr
      << "Unable to open "
      << input_path
      << " for reading."
      << std::endl;
    std::exit(EXIT_FAILURE);
  }

  try
  {
    program.compile(laskin::quote::parse(input), options);
    program.transpile(writer, options);
  }
  catch (const laskin::error& e)
  {
    input.close();
    std::cerr << e << std::endl;
    std::exit(EXIT_FAILURE);
  }
  input.close();

  if (!output_path && !output_path_set)
  {
    output_path = input_path;
    output_path->replace_extension(".cpp");
  }

  if (output_path)
  {
    std::ofstream output(*output_path);

    if (!output.good())
    {
      std::cerr
        << "Unable to open "
        << *output_path
        << " for writing."
        << std::endl;
      std::exit(EXIT_FAILURE);
    }
    output << writer << std::endl;
    output.close();
  } else {
    std::cout << writer << std::endl;
  }

  return EXIT_SUCCESS;
}

static void
parse_args(int argc, char** argv)
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
      input_path = arg;
      break;
    }
    else if (!arg[1])
    {
      break;
    }
    else if (arg[1] == '-')
    {
      if (!std::strcmp(arg, "--no-number-optimization"))
      {
        options.number_optimization = false;
        continue;
      }
      else if (!std::strcmp(arg, "--no-push-define-optimization"))
      {
        options.push_define_optimization = false;
        continue;
      }
      else if (!std::strcmp(arg, "--help"))
      {
        print_usage(std::cout, argv[0]);
        std::exit(EXIT_SUCCESS);
      }
      else if (!std::strcmp(arg, "--version"))
      {
        std::cerr << "Laskin " << LASKIN_VERSION << std::endl;
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
        case 'o':
          if (offset < argc)
          {
            const auto arg = argv[offset++];

            output_path_set = true;
            if (std::strcmp(arg, "-"))
            {
              output_path = arg;
            }
          } else {
            std::cerr << "Argument expected for the -o option." << std::endl;
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

  if (input_path.empty())
  {
    std::cerr << "Missing filename." << std::endl;
    print_usage(std::cerr, argv[0]);
    std::exit(EXIT_FAILURE);
  }
  else if (offset < argc)
  {
    std::cerr << "Too many arguments given." << std::endl;
    print_usage(std::cerr, argv[0]);
    std::exit(EXIT_FAILURE);
  }
}

static void
print_usage(std::ostream& output, const char* executable_name)
{
  output
    << std::endl
    << "Usage: "
    << executable_name
    << " [switches] <filename>"
    << std::endl
    << "  -o filename       Where the transpiled C++ will be written to."
    << std::endl
    << "  --no-number-optimization"
    << std::endl
    << "  --version         Print the version."
    << std::endl
    << "  --help            Display this message."
    << std::endl
    << std::endl;
}
