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
  static void w_true(class context& context, std::ostream&)
  {
    context << value::make_boolean(true);
  }

  static void w_false(class context& context, std::ostream&)
  {
    context << value::make_boolean(false);
  }

  static void w_eq(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << value::make_boolean(a == b);
  }

  static void w_ne(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << value::make_boolean(a != b);
  }

  static void w_gt(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << value::make_boolean(a > b);
  }

  static void w_lt(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << value::make_boolean(a < b);
  }

  static void w_gte(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << value::make_boolean(a >= b);
  }

  static void w_lte(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << value::make_boolean(a <= b);
  }

  static void w_add(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << a + b;
  }

  static void w_sub(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << a - b;
  }

  static void w_clear(class context& context, std::ostream&)
  {
    context.clear();
  }

  static void w_dup(class context& context, std::ostream&)
  {
    context << context.peek();
  }

  static void w_drop(class context& context, std::ostream&)
  {
    context.pop();
  }

  static void w_nip(class context& context, std::ostream&)
  {
    const auto a = context.pop();

    context.pop();
    context.push(a);
  }

  static void w_over(class context& context, std::ostream&)
  {
    const auto a = context.pop();
    const auto b = context.pop();

    context << b << a << b;
  }

  static void w_rot(class context& context, std::ostream&)
  {
    const auto a = context.pop();
    const auto b = context.pop();
    const auto c = context.pop();

    context << b << a << c;
  }

  static void w_swap(class context& context, std::ostream&)
  {
    const auto a = context.pop();
    const auto b = context.pop();

    context << a << b;
  }

  static void w_tuck(class context& context, std::ostream&)
  {
    const auto a = context.pop();
    const auto b = context.pop();

    context << a << b << a;
  }

  static void w_print(class context& context, std::ostream& out)
  {
    out << context.pop() << std::endl;
  }

  static void w_quit(class context& context, std::ostream&)
  {
    std::exit(EXIT_SUCCESS);
  }

  static void w_if(class context& context, std::ostream& out)
  {
    const auto quote = context.pop().as_quote();
    const auto condition = context.pop().as_boolean();

    if (condition)
    {
      quote.call(context, out);
    }
  }

  static void w_if_else(class context& context, std::ostream& out)
  {
    const auto else_quote = context.pop().as_quote();
    const auto then_quote = context.pop().as_quote();
    const auto condition = context.pop().as_boolean();

    if (condition)
    {
      then_quote.call(context, out);
    } else {
      else_quote.call(context, out);
    }
  }

  namespace api
  {
    extern "C" const context::dictionary_definition utils =
    {
      // Constants.
      { U"true", w_true },
      { U"false", w_false },

      // Common operators.
      { U"=", w_eq },
      { U"<>", w_ne },
      { U">", w_gt },
      { U"<", w_lt },
      { U">=", w_gte },
      { U"<=", w_lte },
      { U"+", w_add },
      { U"-", w_sub },

      // Stack manipulation.
      { U"clear", w_clear },
      { U"dup", w_dup },
      { U"drop", w_drop },
      { U"nip", w_nip },
      { U"over", w_over },
      { U"rot", w_rot },
      { U"swap", w_swap },
      { U"tuck", w_tuck },

      // I/O.
      { U".", w_print },

      // Program logic.
      { U"quit", w_quit },
      { U"if", w_if },
      { U"if-else", w_if_else }
    };
  }
}
