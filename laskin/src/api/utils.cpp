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

#include "laskin/context.hpp"
#include "laskin/error.hpp"

using namespace laskin;

/**
 * = ( any any -- boolean )
 *
 * Tests whether the given values are equal.
 */
LASKIN_BUILTIN_WORD(w_eq)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << (a == b);
}

/**
 * <> ( any any -- boolean )
 *
 * Tests whether the given values are not equal.
 */
LASKIN_BUILTIN_WORD(w_ne)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << (a != b);
}

/**
 * > ( any any -- boolean )
 *
 * Performs greater than comparison on the values.
 */
LASKIN_BUILTIN_WORD(w_gt)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << (a > b);
}

/**
 * < ( any any -- boolean )
 *
 * Performs less than comparison on the values.
 */
LASKIN_BUILTIN_WORD(w_lt)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << (a < b);
}

/**
 * >= ( any any -- boolean )
 *
 * Performs greater than or equals comparison on the values.
 */
LASKIN_BUILTIN_WORD(w_gte)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << (a >= b);
}

/**
 * <= ( any any -- boolean )
 *
 * Performs less than or equals comparison on the values.
 */
LASKIN_BUILTIN_WORD(w_lte)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << (a <= b);
}

/**
 * + ( any any -- any )
 *
 * Performs addition on the values.
 */
LASKIN_BUILTIN_WORD(w_add)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << a + b;
}

/**
 * - ( any any -- any )
 *
 * Performs substraction on the values.
 */
LASKIN_BUILTIN_WORD(w_sub)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << a - b;
}

/**
 * * ( any any -- any )
 *
 * Performs multiplication on the values.
 */
LASKIN_BUILTIN_WORD(w_mul)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << a * b;
}

/**
 * / ( any any -- any )
 *
 * Performs division on the values.
 */
LASKIN_BUILTIN_WORD(w_div)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << a / b;
}

/**
 * % ( any any -- any )
 *
 * Performs modulation on the values.
 */
LASKIN_BUILTIN_WORD(w_mod)
{
  const auto b = context.pop();
  const auto a = context.pop();

  context << a % b;
}

static inline void
type_test(class context& context, enum value::type type)
{
  context << context.peek().is(type);
}

/**
 * boolean? ( any -- any boolean )
 *
 * Tests whether given value is boolean.
 */
LASKIN_BUILTIN_WORD(w_is_boolean)
{
  type_test(context, value::type::boolean);
}

/**
 * date? ( any -- any boolean )
 *
 * Tests whether given value is date.
 */
LASKIN_BUILTIN_WORD(w_is_date)
{
  type_test(context, value::type::date);
}

/**
 * month? ( any -- any boolean )
 *
 * Tests whether given value is month.
 */
LASKIN_BUILTIN_WORD(w_is_month)
{
  type_test(context, value::type::month);
}

/**
 * number? ( any -- any boolean )
 *
 * Tests whether given value is number.
 */
LASKIN_BUILTIN_WORD(w_is_number)
{
  type_test(context, value::type::number);
}

/**
 * vector? ( any -- any boolean )
 *
 * Tests whether given value is vector.
 */
LASKIN_BUILTIN_WORD(w_is_vector)
{
  type_test(context, value::type::vector);
}

/**
 * record? ( any -- any boolean )
 *
 * Tests whether given value is record.
 */
LASKIN_BUILTIN_WORD(w_is_record)
{
  type_test(context, value::type::record);
}

/**
 * string? ( any -- any boolean )
 *
 * Tests whether given value is string.
 */
LASKIN_BUILTIN_WORD(w_is_string)
{
  type_test(context, value::type::string);
}

/**
 * time? ( any -- any boolean )
 *
 * Tests whether given value is time.
 */
LASKIN_BUILTIN_WORD(w_is_time)
{
  type_test(context, value::type::time);
}

/**
 * quote? ( any -- any boolean )
 *
 * Tests whether given value is quote.
 */
LASKIN_BUILTIN_WORD(w_is_quote)
{
  type_test(context, value::type::quote);
}

/**
 * weekday? ( any -- any boolean )
 *
 * Tests whether given value is weekday.
 */
LASKIN_BUILTIN_WORD(w_is_weekday)
{
  type_test(context, value::type::weekday);
}

/**
 * clear ( -- )
 *
 * Removes every value from the stack.
 */
LASKIN_BUILTIN_WORD(w_clear)
{
  context.clear();
}

/**
 * dup ( any -- any any )
 *
 * Duplicates topmost value of the stack.
 */
LASKIN_BUILTIN_WORD(w_dup)
{
  context << context.peek();
}

/**
 * drop ( any -- )
 *
 * Removes topmost value of the stack.
 */
LASKIN_BUILTIN_WORD(w_drop)
{
  context.pop();
}

/**
 * nip ( any any -- any )
 *
 * Drops the first value and pushes the second value onto stack.
 */
LASKIN_BUILTIN_WORD(w_nip)
{
  const auto a = context.pop();

  context.pop();
  context.push(a);
}

/**
 * over ( any any -- any any any )
 *
 * Copies second topmost value of the stack into topmost value of the
 * stack.
 */
LASKIN_BUILTIN_WORD(w_over)
{
  const auto a = context.pop();
  const auto b = context.pop();

  context << b << a << b;
}

/**
 * rot ( any any any -- any any any )
 *
 * Rotates the three topmost values on the stack.
 */
LASKIN_BUILTIN_WORD(w_rot)
{
  const auto a = context.pop();
  const auto b = context.pop();
  const auto c = context.pop();

  context << b << a << c;
}

/**
 * swap ( any any -- any any )
 *
 * Swaps positions of the two topmost values on the stack.
 */
LASKIN_BUILTIN_WORD(w_swap)
{
  const auto a = context.pop();
  const auto b = context.pop();

  context << a << b;
}

/**
 * tuck ( any any -- any any any )
 *
 * Copies the topmost value of the stack as the third topmost value of the
 * stack.
 */
LASKIN_BUILTIN_WORD(w_tuck)
{
  const auto a = context.pop();
  const auto b = context.pop();

  context << a << b << a;
}

/**
 * depth ( -- number )
 *
 * Returns the number of values in the stack.
 */
LASKIN_BUILTIN_WORD(w_depth)
{
  context << value::make_number(
    static_cast<std::int64_t>(context.data.size())
  );
}

/**
 * >string ( any -- string )
 *
 * Returns textual representation of the topmost value of the stack.
 */
LASKIN_BUILTIN_WORD(w_to_string)
{
  context << context.pop().to_string();
}

/**
 * >source ( any -- string )
 *
 * Converts topmost value of the stack into source code.
 */
LASKIN_BUILTIN_WORD(w_to_source)
{
  context << context.pop().to_source();
}

/**
 * . ( any -- )
 *
 * Outputs textual presentation of the topmost value of the stack with a
 * newline after it.
 */
LASKIN_BUILTIN_WORD(w_println)
{
  const auto value = context.pop();

  if (out)
  {
    *out << value << std::endl;
  }
}

/**
 * .. ( any -- )
 *
 * Outputs textual presentation of the topmost value of the stack without
 * printing a newline afterwards.
 */
LASKIN_BUILTIN_WORD(w_print)
{
  const auto value = context.pop();

  if (out)
  {
    *out << value;
  }
}

/**
 * .s ( -- )
 *
 * Outputs textual presentation of first ten values in the stack to the
 * output.
 */
LASKIN_BUILTIN_WORD(w_stack_preview)
{
  const auto& data = context.data;
  const auto size = data.size();

  if (!out)
  {
    return;
  }
  if (!size)
  {
    *out << "Stack is empty." << std::endl;
    return;
  }
  for (std::deque<value>::size_type i = 0; i < size && i < 10; ++i)
  {
    const auto& value = data[size - i - 1];

    *out
      << (size - i)
      << ": "
      << peelo::unicode::encoding::utf8::encode(value.to_source())
      << std::endl;
  }
}

/**
 * quit ( -- )
 *
 * Terminates the interpreter.
 */
LASKIN_BUILTIN_WORD(w_quit)
{
  throw error(error::type::exit, U"Program exit");
}

/**
 * if ( quote boolean -- )
 *
 * Executes quote if given boolean value is true.
 */
LASKIN_BUILTIN_WORD(w_if)
{
  const auto quote = context.pop().as_quote();
  const auto condition = context.pop().as_boolean();

  if (condition)
  {
    quote.call(context, out);
  }
}

/**
 * if-else ( quote quote boolean -- )
 *
 * Executes first quote if given boolean value is true and the second one
 * otherwise.
 */
LASKIN_BUILTIN_WORD(w_if_else)
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

/**
 * while ( quote quote -- )
 *
 * Executes first quote as long as second quote pushes true into the stack.
 */
LASKIN_BUILTIN_WORD(w_while)
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

/**
 * try ( quote quote -- )
 *
 * Executes first quote if second quote throws an error.
 */
LASKIN_BUILTIN_WORD(w_try)
{
  const auto quote = context.pop().as_quote();
  const auto catch_quote = context.pop().as_quote();

  try
  {
    quote.call(context, out);
  }
  catch (const error& e)
  {
    // Allow program exit to be passed through.
    if (e.is(error::type::exit))
    {
      throw e;
    }
    context << e.message;
    catch_quote.call(context, out);
  }
}

/**
 * try-else ( quote quote quote -- )
 *
 * Executes first quote if third quote does not throw error and second quote
 * otherwise.
 */
LASKIN_BUILTIN_WORD(w_try_else)
{
  const auto quote = context.pop().as_quote();
  const auto catch_quote = context.pop().as_quote();
  const auto else_quote = context.pop().as_quote();

  try
  {
    quote.call(context, out);
  }
  catch (const error& e)
  {
    // Allow program exit to be passed through.
    if (e.is(error::type::exit))
    {
      throw e;
    }
    context << e.message;
    catch_quote.call(context, out);
    return;
  }
  else_quote.call(context, out);
}

/**
 * lookup ( string -- quote )
 *
 * Searches for an word from the dictionary and returns quote associated with
 * it.
 *
 * Name error will be thrown if dictionary does not contain such entry.
 */
LASKIN_BUILTIN_WORD(w_lookup)
{
  const auto& dictionary = context.dictionary;
  const auto id = context.pop().as_string();
  const auto word = dictionary.find(id);

  if (word != std::end(dictionary))
  {
    context << word->second;
    return;
  }

  throw error(error::type::name, U"Unrecognized symbol: `" + id + U"'");
}

/**
 * define ( quote string -- )
 *
 * Inserts an word into the dictionary where quote is associated with the
 * name given as string.
 */
LASKIN_BUILTIN_WORD(w_define)
{
  const auto id = context.pop().as_string();
  const auto value = context.pop();

  context.dictionary[id] = value;
}

/**
 * delete ( string -- )
 *
 * Removes an word from the dictionary.
 *
 * Name error will be thrown if dictionary does not contain such entry.
 */
LASKIN_BUILTIN_WORD(w_delete)
{
  const auto id = context.pop().as_string();
  auto& dictionary = context.dictionary;
  const auto i = dictionary.find(id);

  if (i != std::end(dictionary))
  {
    dictionary.erase(i);
  } else {
    throw error(error::type::name, U"Unrecognized symbol: `" + id + U"'");
  }
}

/**
 * symbols ( -- vector )
 *
 * Constructs vector containing all words in the dictionary as strings.
 */
LASKIN_BUILTIN_WORD(w_symbols)
{
  const auto& dictionary = context.dictionary;
  value::vector_container result;

  result.reserve(dictionary.size());
  for (const auto& entry : dictionary)
  {
    result.push_back(value::make_string(entry.first));
  }
  context << result;
}

/**
 * include ( string -- )
 *
 * Searches for the file system for a while which path is given as string and
 * executes it as Laskin program.
 */
LASKIN_BUILTIN_WORD(w_include)
{
  using peelo::unicode::encoding::utf8::encode;

  const auto path = context.pop().as_string();

  context.include(encode(path), out);
}

namespace laskin::api
{
  extern "C" const context::dictionary_definition utils =
  {
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
    { U"%", w_mod },

    // Stack testing.
    { U"boolean?", w_is_boolean },
    { U"date?", w_is_date },
    { U"month?", w_is_month },
    { U"number?", w_is_number },
    { U"vector?", w_is_vector },
    { U"record?", w_is_record },
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
    { U".", w_println },
    { U"..", w_print },
    { U".s", w_stack_preview },

    // Program logic.
    { U"quit", w_quit },
    { U"if", w_if },
    { U"if-else", w_if_else },
    { U"while", w_while },
    { U"try", w_try },
    { U"try-else", w_try_else },

    // Dictionary related.
    { U"lookup", w_lookup },
    { U"define", w_define },
    { U"delete", w_delete },
    { U"symbols", w_symbols },

    // Importing stuff from the file system.
    { U"include", w_include }
  };
}
