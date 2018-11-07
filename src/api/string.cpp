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
#include <peelo/unicode.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"

namespace laskin
{
  static void w_length(class context& context, std::ostream&)
  {
    context << value::make_number(
      mpf_class(context.peek().as_string().length())
    );
  }

  static void w_reverse(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();

    context << value::make_string(string.rbegin(), string.rend());
  }

  static void convert_string(class context& context,
                             char32_t (*callback)(char32_t))
  {
    auto string = context.pop().as_string();

    for (auto& c : string)
    {
      c = callback(c);
    }
    context << value::make_string(string);
  }

  static void w_lower_case(class context& context, std::ostream&)
  {
    convert_string(context, peelo::unicode::tolower);
  }

  static void w_upper_case(class context& context, std::ostream&)
  {
    convert_string(context, peelo::unicode::toupper);
  }

  static void w_at(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    const auto length = string.length();
    auto index = context.pop().as_number().to_long();
    char32_t c;

    if (index < 0)
    {
      index += length;
    }
    if (!length || index < 0 || index >= static_cast<long>(length))
    {
      throw error(error::type::range, U"String index out of bounds.");
    }
    c = string[index];
    context << value::make_string(std::u32string(&c, 1));
  }

  namespace api
  {
    extern "C" const context::dictionary_definition string =
    {
      { U"string:length", w_length },

      // Modifications.
      { U"string:reverse", w_reverse },
      { U"string:lower-case", w_lower_case },
      { U"string:upper-case", w_upper_case },

      { U"string:@", w_at }
    };
  }
}
