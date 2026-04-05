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
#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/error.hpp"

namespace laskin
{
  error::error(
    enum type type_,
    const std::u32string& message_,
    const std::optional<struct position>& position_
  )
    : type(type_)
    , message(peelo::unicode::encoding::utf8::encode(message_))
    , position(position_) {}

  std::ostream&
  operator<<(std::ostream& os, enum error::type type)
  {
    switch (type)
    {
      case error::type::syntax:
        os << "Syntax error";
        break;

      case error::type::type:
        os << "Type error";
        break;

      case error::type::unit:
        os << "Unit error";
        break;

      case error::type::range:
        os << "Range error";
        break;

      case error::type::domain:
        os << "Domain error";
        break;

      case error::type::name:
        os << "Name error";
        break;

      case error::type::system:
        os << "System error";
        break;

      case error::type::exit:
        os << "Program exit";
        break;
    }

    return os;
  }

  std::ostream&
  operator<<(std::ostream& os, const class error& error)
  {
    if (error.position)
    {
      os << *error.position << ": ";
    }
    os << error.type;
    if (!error.message.empty())
    {
      os << ": " << error.message;
    }

    return os;
  }
}
