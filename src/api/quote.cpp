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
  static void w_call(class context& context, std::ostream& out)
  {
    context.pop().as_quote().call(context, out);
  }

  static void w_compose(class context& context, std::ostream& out)
  {
    const auto right = context.pop().as_quote();
    const auto left = context.pop().as_quote();
    const auto call = std::make_shared<node::symbol>(U"quote:call");

    context << value::make_quote({
      std::make_shared<node::literal>(value::make_quote(left)),
      call,
      std::make_shared<node::literal>(value::make_quote(right)),
      call
    });
  }

  static void w_curry(class context& context, std::ostream&)
  {
    const auto quote = context.pop().as_quote();
    const auto argument = context.pop();

    context << value::make_quote({
      std::make_shared<node::literal>(argument),
      std::make_shared<node::literal>(value::make_quote(quote)),
      std::make_shared<node::symbol>(U"quote:call")
    });
  }

  static void w_negate(class context& context, std::ostream&)
  {
    const auto quote = context.pop().as_quote();

    context << value::make_quote({
      std::make_shared<node::literal>(value::make_quote(quote)),
      std::make_shared<node::symbol>(U"quote:call"),
      std::make_shared<node::symbol>(U"boolean:not")
    });
  }

  static void w_dip(class context& context, std::ostream& out)
  {
    const auto quote = context.pop().as_quote();
    const auto value = context.pop();

    quote.call(context, out);
    context << value;
  }

  namespace api
  {
    extern "C" const context::dictionary_definition quote =
    {
      { U"quote:call", w_call },
      { U"quote:compose", w_compose },
      { U"quote:curry", w_curry },
      { U"quote:negate", w_negate },
      { U"quote:dip", w_dip }
    };
  }
}
