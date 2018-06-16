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
#include "laskin/context.hpp"

namespace laskin
{
  static void w_add(class context& context, std::ostream&)
  {
    const auto a = context.pop();
    const auto b = context.pop();

    context << value::make_number(a.as_number() + b.as_number());
  }
  static void w_sub(class context& context, std::ostream& out)
  {
    const auto a = context.pop();
    const auto b = context.pop();

    context << value::make_number(a.as_number() - b.as_number());
  }

  static void w_mul(class context& context, std::ostream&)
  {
    const auto a = context.pop();
    const auto b = context.pop();

    context << value::make_number(a.as_number() * b.as_number());
  }

  static void w_range(class context& context, std::ostream&)
  {
    const auto limit = context.pop().as_number();
    auto current = context.pop().as_number();
    std::vector<value> result;

    while (current < limit)
    {
      result.push_back(value::make_number(current));
      current += 1;
    }
    context << value::make_vector(result);
  }

  namespace api
  {
    extern "C" const context::dictionary_definition number =
    {
      { U"number:+", w_add },
      { U"number:-", w_sub },
      { U"number:*", w_mul },
      // TODO: { U"number:/", w_div }

      { U"number:range", w_range }
    };
  }
}
