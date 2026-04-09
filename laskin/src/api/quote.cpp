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
#include "laskin/context.hpp"

using namespace laskin;

/**
 * quote:call ( quote -- )
 *
 * Executes quote.
 */
LASKIN_BUILTIN_WORD(w_call)
{
  context.pop().as_quote().call(context, out);
}

/**
 * quote:compose ( quote quote -- quote )
 *
 * Constructs quote that calls two quotes.
 */
LASKIN_BUILTIN_WORD(w_compose)
{
  const auto right = context.pop().as_quote();
  const auto left = context.pop().as_quote();
  const auto call = std::make_shared<node::symbol>(U"quote:call");

  context << quote({
    std::make_shared<node::literal>(left),
    call,
    std::make_shared<node::literal>(right),
    call
  });
}

/**
 * quote:curry ( any quote -- quote )
 *
 * Constructs quote that curries given value as argument for the quote.
 */
LASKIN_BUILTIN_WORD(w_curry)
{
  const auto q = context.pop().as_quote();
  const auto argument = context.pop();

  context << quote({
    std::make_shared<node::literal>(argument),
    std::make_shared<node::literal>(q),
    std::make_shared<node::symbol>(U"quote:call")
  });
}

/**
 * quote:negate ( quote -- quote )
 *
 * Constructs quote that negates result of given quote.
 */
LASKIN_BUILTIN_WORD(w_negate)
{
  const auto q = context.pop().as_quote();

  context << quote({
    std::make_shared<node::literal>(q),
    std::make_shared<node::symbol>(U"quote:call"),
    std::make_shared<node::symbol>(U"boolean:not")
  });
}

/**
 * quote:dip ( any quote -- any )
 *
 * Temporarily hides given value from the stack and calls given quote. Once
 * the quote has returned from it's execution, hidden value will be placed
 * back on the stack.
 */
LASKIN_BUILTIN_WORD(w_dip)
{
  const auto quote = context.pop().as_quote();
  const auto value = context.pop();

  quote.call(context, out);
  context << value;
}

namespace laskin::api
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
