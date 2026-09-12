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

  std::u32string
  to_string(enum error::type type)
  {
    switch (type)
    {
      case error::type::syntax:
        return U"Syntax error";

      case error::type::type:
        return U"Type error";

      case error::type::unit:
        return U"Unit error";

      case error::type::range:
        return U"Range error";

      case error::type::domain:
        return U"Domain error";

      case error::type::name:
        return U"Name error";

      case error::type::system:
        return U"System error";

      case error::type::exit:
        return U"Program exit";
    }

    return U"Unknown error";
  }

  std::u32string
  to_string(const error& error)
  {
    using peelo::unicode::encoding::utf8::decode;

    std::u32string result;

    if (error.position)
    {
      result.append(to_string(*error.position)).append(U": ");
    }
    result.append(to_string(error.type));
    if (!error.message.empty())
    {
      result.append(U": ").append(decode(error.message));
    }

    return result;
  }

  std::u32string
  to_source(enum error::type type)
  {
    switch (type)
    {
      case error::type::syntax:
        return U"syntax";

      case error::type::type:
        return U"type";

      case error::type::unit:
        return U"unit";

      case error::type::range:
        return U"range";

      case error::type::domain:
        return U"domain";

      case error::type::name:
        return U"name";

      case error::type::system:
        return U"system";

      case error::type::exit:
        return U"exit";
    }

    return U"unknown";
  }

  std::ostream&
  operator<<(std::ostream& os, const error& error)
  {
    using peelo::unicode::encoding::utf8::encode;

    os << encode(to_string(error));

    return os;
  }
}
