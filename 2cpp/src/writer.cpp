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
#include <laskin/utils.hpp>
#include <peelo/unicode/encoding/utf8.hpp>

#include "./writer.hpp"

namespace laskin2cpp
{
  static const std::vector<std::string> header =
  {
    "#include <laskin/context.hpp>",
    "#include <laskin/error.hpp>",
    "#include <laskin/quote.hpp>",
    "",
    "using namespace laskin;",
    "",
    "int",
    "main(int argc, char** argv)",
    "{",
    "  context c;",
    "",
    "  try",
    "  {",
  };

  static const std::vector<std::string> footer =
  {
    "  }",
    "  catch (const error& e)",
    "  {",
    "    if (e.is(error::type::exit))",
    "    {",
    "      std::exit(EXIT_SUCCESS);",
    "    } else {",
    "      std::cerr << e << std::endl;",
    "      std::exit(EXIT_FAILURE);",
    "    }",
    "  }",
    "  ",
    "  return EXIT_SUCCESS;",
    "}"
  };

  std::string
  writer::escape(const std::u32string& input)
  {
    using laskin::utils::escape_string;
    using peelo::unicode::encoding::utf8::encode;

    return "U" + encode(escape_string(input));
  }

  void
  writer::flush()
  {
    if (!m_buffer.empty())
    {
      m_lines.push_back(m_buffer);
      m_buffer.clear();
    }
  }

  void
  writer::print(const std::string& line)
  {
    if (m_buffer.empty())
    {
      m_buffer.append(m_indent * 2, ' ');
    }
    m_buffer.append(line);
  }

  void
  writer::println(const std::string& line)
  {
    print(line);
    flush();
  }

  std::ostream&
  operator<<(std::ostream& os, const class writer& writer)
  {
    for (const auto& line : header)
    {
      os << line << std::endl;
    }
    for (const auto& line : writer)
    {
      os << line << std::endl;
    }
    for (const auto& line : footer)
    {
      os << line << std::endl;
    }

    return os;
  }
}
