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
#include <cmath>
#include <fstream>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"

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

  static void w_pi(class context& context, std::ostream&)
  {
    context << value::make_number(M_PI);
  }

  static void w_e(class context& context, std::ostream&)
  {
    context << value::make_number(M_E);
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

  static void w_mul(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << a * b;
  }

  static void w_div(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << a / b;
  }

  static void w_max(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << (a > b ? a : b);
  }

  static void w_min(class context& context, std::ostream&)
  {
    const auto b = context.pop();
    const auto a = context.pop();

    context << (a < b ? a : b);
  }

  static void type_test(class context& context, enum value::type type)
  {
    context << value::make_boolean(context.peek().is(type));
  }

  static void w_is_boolean(class context& context, std::ostream&)
  {
    type_test(context, value::type::boolean);
  }

  static void w_is_date(class context& context, std::ostream&)
  {
    type_test(context, value::type::date);
  }

  static void w_is_month(class context& context, std::ostream&)
  {
    type_test(context, value::type::month);
  }

  static void w_is_number(class context& context, std::ostream&)
  {
    type_test(context, value::type::number);
  }

  static void w_is_vector(class context& context, std::ostream&)
  {
    type_test(context, value::type::vector);
  }

  static void w_is_string(class context& context, std::ostream&)
  {
    type_test(context, value::type::string);
  }

  static void w_is_time(class context& context, std::ostream&)
  {
    type_test(context, value::type::time);
  }

  static void w_is_quote(class context& context, std::ostream&)
  {
    type_test(context, value::type::quote);
  }

  static void w_is_weekday(class context& context, std::ostream&)
  {
    type_test(context, value::type::weekday);
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

  static void w_depth(class context& context, std::ostream&)
  {
    context << value::make_number(context.data().size());
  }

  static void w_to_string(class context& context, std::ostream&)
  {
    context << value::make_string(context.pop().to_string());
  }

  static void w_to_source(class context& context, std::ostream&)
  {
    context << value::make_string(context.pop().to_source());
  }

  static void w_print(class context& context, std::ostream& out)
  {
    out << context.pop() << std::endl;
  }

  static void w_stack_preview(class context& context, std::ostream& out)
  {
    const auto& data = context.data();
    const auto size = data.size();

    if (!size)
    {
      out << "Stack is empty." << std::endl;
      return;
    }
    for (std::deque<value>::size_type i = 0; i < size && i < 10; ++i)
    {
      const auto& value = data[size - i - 1];

      std::cout
        << (size - i)
        << ": "
        << peelo::unicode::encoding::utf8::encode(value.to_source())
        << std::endl;
    }
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

  static void w_while(class context& context, std::ostream& out)
  {
    const auto quote = context.pop().as_quote();
    const auto condition = context.pop().as_quote();

    for (;;)
    {
      condition.call(context, out);
      if (!context.pop().as_boolean())
      {
        return;
      }
      quote.call(context, out);
    }
  }

  static void w_lookup(class context& context, std::ostream&)
  {
    const auto& dictionary = context.dictionary();
    const auto id = context.pop().as_string();
    const auto word = dictionary.find(id);

    if (word != std::end(dictionary))
    {
      context << word->second;
      return;
    }

    throw error(error::type::name, U"Unrecognized symbol: `" + id + U"'");
  }

  static void w_define(class context& context, std::ostream&)
  {
    const auto id = context.pop().as_string();
    const auto value = context.pop();

    context.dictionary()[id] = value;
  }

  static void w_include(class context& context, std::ostream& out)
  {
    using peelo::unicode::encoding::utf8::decode_validate;
    const auto path = context.pop().as_string();
    std::ifstream input(peelo::unicode::encoding::utf8::encode(path));
    std::string raw_source;
    std::u32string source;

    if (!input.good())
    {
      throw error(
        error::type::system,
        U"Unable to open file `" + path + U"' for reading."
      );
    }
    raw_source = std::string(
      std::istreambuf_iterator<char>(input),
      std::istreambuf_iterator<char>()
    );
    input.close();
    if (!decode_validate(raw_source, source))
    {
      throw error(
        error::type::system,
        U"Unable to decode contents of the file with UTF-8 character encoding."
      );
    }
    quote::parse(source).call(context, out);
  }

  namespace api
  {
    extern "C" const context::dictionary_definition utils =
    {
      // Constants.
      { U"true", w_true },
      { U"false", w_false },
      { U"pi", w_pi },
      { U"e", w_e },

      // Common operators.
      { U"=", w_eq },
      { U"<>", w_ne },
      { U">", w_gt },
      { U"<", w_lt },
      { U">=", w_gte },
      { U"<=", w_lte },
      { U"+", w_add },
      { U"-", w_sub },
      { U"*", w_mul },
      { U"/", w_div },
      { U"max", w_max },
      { U"min", w_min },

      // Stack testing.
      { U"boolean?", w_is_boolean },
      { U"date?", w_is_date },
      { U"month?", w_is_month },
      { U"number?", w_is_number },
      { U"vector?", w_is_vector },
      { U"string?", w_is_string },
      { U"time?", w_is_time },
      { U"quote?", w_is_quote },
      { U"weekday?", w_is_weekday },

      // Stack manipulation.
      { U"clear", w_clear },
      { U"dup", w_dup },
      { U"drop", w_drop },
      { U"nip", w_nip },
      { U"over", w_over },
      { U"rot", w_rot },
      { U"swap", w_swap },
      { U"tuck", w_tuck },
      { U"depth", w_depth },

      // Conversions.
      { U">string", w_to_string },
      { U">source", w_to_source },

      // I/O.
      { U".", w_print },
      { U".s", w_stack_preview },

      // Program logic.
      { U"quit", w_quit },
      { U"if", w_if },
      { U"if-else", w_if_else },
      { U"while", w_while },

      // Dictionary related.
      { U"lookup", w_lookup },
      { U"define", w_define },

      // Importing stuff from the file system.
      { U"include", w_include }
    };
  }
}
