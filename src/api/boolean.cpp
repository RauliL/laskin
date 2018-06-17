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
  static void w_not(class context& context, std::ostream&)
  {
    context.push(value::make_boolean(!context.pop().as_boolean()));
  }

  static void w_and(class context& context, std::ostream&)
  {
    const auto b = context.pop().as_boolean();
    const auto a = context.pop().as_boolean();

    context.push(value::make_boolean(a && b));
  }

  static void w_or(class context& context, std::ostream&)
  {
    const auto b = context.pop().as_boolean();
    const auto a = context.pop().as_boolean();

    context.push(value::make_boolean(a || b));
  }

  static void w_xor(class context& context, std::ostream&)
  {
    const auto b = context.pop().as_boolean();
    const auto a = context.pop().as_boolean();

    context.push(value::make_boolean(a != b && (a || b)));
  }

  namespace api
  {
    extern "C" const context::dictionary_definition boolean =
    {
      { U"boolean:not", w_not },
      { U"boolean:and", w_and },
      { U"boolean:or", w_or },
      { U"boolean:xor", w_xor }
    };
  }
}
