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
#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/error.hpp"

namespace laskin
{
  error::error(enum type type,
               const std::u32string& message,
               int line,
               int column)
    : m_type(type)
    , m_message(peelo::unicode::encoding::utf8::encode(message))
    , m_line(line)
    , m_column(column) {}

  std::u32string error::type_description(enum type type)
  {
    switch (type)
    {
    case type::syntax:
      return U"Syntax error";

    case type::type:
      return U"Type error";

    case type::unit:
      return U"Unit error";

    case type::range:
      return U"Range error";

    case type::domain:
      return U"Domain error";

    case type::name:
      return U"Name error";

    case type::system:
      return U"System error";
    }

    return U"Unknown error"; // Just to keep GCC happy.
  }

  std::ostream& operator<<(std::ostream& out, const class error& error)
  {
    const auto line = error.line();
    const auto& message = error.message();

    if (line != 0)
    {
      out << line << ':' << error.column() << ':';
    }
    out << peelo::unicode::encoding::utf8::encode(
      error::type_description(error.type())
    );
    if (!message.empty())
    {
      out << ": " << message;
    }

    return out;
  }
}
