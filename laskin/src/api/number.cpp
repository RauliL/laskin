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
#include <cmath>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/macros.hpp"

using namespace laskin;

/**
 * number:has-unit? ( number -- number boolean )
 *
 * Tests whether number has measurement unit.
 */
BUILTIN_WORD(w_has_unit)
{
  context << bool(context.peek().as_number().measurement_unit());
}

/**
 * number:unit ( number -- number string )
 *
 * Returns symbol of the measurement unit that the number has. For example
 * kilometer would be "km", minutes "min" and so on.
 *
 * Unit error will be thrown if the number does not have measurement unit.
 */
BUILTIN_WORD(w_unit)
{
  const auto& unit = context.peek().as_number().measurement_unit();

  if (!unit)
  {
    throw error(error::type::unit, U"Value has no measurement unit.");
  }
  context << unit->symbol();
}

/**
 * number:unit-type ( number -- number string )
 *
 * Returns textual description of the measurement unit type that the number
 * has. For example kilometer would return "length", kilogram would return
 * "weight" and minutes would return "time".
 *
 * Unit error will be thrown if the number does not have measurement unit.
 */
BUILTIN_WORD(w_unit_type)
{
  const auto& unit = context.peek().as_number().measurement_unit();

  if (!unit)
  {
    throw error(error::type::unit, U"Value has no measurement unit.");
  }
  context << to_string(unit->type());
}

/**
 * number:drop-unit ( number -- number )
 *
 * Removes the measurement unit from the number, if it has one.
 */
BUILTIN_WORD(w_drop_unit)
{
  context << context.pop().as_number().without_unit();
}

/**
 * number:range ( number number -- vector )
 *
 * Constructs an vector of numbers within between the given minimum and
 * maximum boundaries. For example `1 3 range` would return vector containing
 * numbers 1 and 2.
 */
BUILTIN_WORD(w_range)
{
  const auto limit = context.pop().as_number();
  auto current = context.pop().as_number();
  std::vector<value> result;

  while (current < limit)
  {
    result.push_back(value::make_number(current));
    ++current;
  }
  context << result;
}

/**
 * number:clamp ( number number number -- number )
 *
 * Ensures that number is between given minimum and maximum boundaries.
 */
BUILTIN_WORD(w_clamp)
{
  const auto value = context.pop().as_number();
  const auto max = context.pop().as_number();
  const auto min = context.pop().as_number();

  context << (value > max ? max : value < min ? min : value);
}

/**
 * number:times ( quote number -- )
 *
 * Executes quote given number of times.
 */
BUILTIN_WORD(w_times)
{
  auto count = context.pop().as_number();
  const auto quote = context.pop().as_quote();

  if (count < number())
  {
    count = -count;
  }
  while (count)
  {
    --count;
    quote.call(context, out);
  }
}

BUILTIN_WORD(w_exp)
{
  context << context.pop().as_number().exp();
}

BUILTIN_WORD(w_exp2)
{
  context << context.pop().as_number().exp2();
}

BUILTIN_WORD(w_expm1)
{
  context << context.pop().as_number().expm1();
}

BUILTIN_WORD(w_log)
{
  context << context.pop().as_number().log();
}

BUILTIN_WORD(w_log10)
{
  context << context.pop().as_number().log10();
}

BUILTIN_WORD(w_log2)
{
  context << context.pop().as_number().log2();
}

BUILTIN_WORD(w_log1p)
{
  context << context.pop().as_number().log1p();
}

BUILTIN_WORD(w_pow)
{
  const auto a = context.pop().as_number();
  const auto b = context.pop().as_number();

  context << a.pow(b);
}

BUILTIN_WORD(w_sqrt)
{
  context << context.pop().as_number().sqrt();
}

BUILTIN_WORD(w_cbrt)
{
  context << context.pop().as_number().cbrt();
}

BUILTIN_WORD(w_hypot)
{
  const auto a = context.pop().as_number();
  const auto b = context.pop().as_number();

  context << a.hypot(b);
}

BUILTIN_WORD(w_acos)
{
  context << context.pop().as_number().acos();
}

BUILTIN_WORD(w_asin)
{
  context << context.pop().as_number().asin();
}

BUILTIN_WORD(w_atan)
{
  context << context.pop().as_number().atan();
}

BUILTIN_WORD(w_atan2)
{
  const auto a = context.pop().as_number();
  const auto b = context.pop().as_number();

  context << a.atan2(b);
}

BUILTIN_WORD(w_cos)
{
  context << context.pop().as_number().cos();
}

BUILTIN_WORD(w_sin)
{
  context << context.pop().as_number().sin();
}

BUILTIN_WORD(w_tan)
{
  context << context.pop().as_number().tan();
}

BUILTIN_WORD(w_sinh)
{
  context << context.pop().as_number().sinh();
}

BUILTIN_WORD(w_cosh)
{
  context << context.pop().as_number().cosh();
}

BUILTIN_WORD(w_tanh)
{
  context << context.pop().as_number().tanh();
}

BUILTIN_WORD(w_asinh)
{
  context << context.pop().as_number().asinh();
}

BUILTIN_WORD(w_acosh)
{
  context << context.pop().as_number().acosh();
}

BUILTIN_WORD(w_atanh)
{
  context << context.pop().as_number().atanh();
}

BUILTIN_WORD(w_deg)
{
  const auto value = context.pop().as_number();

  context << (value * 180 / M_PI);
}

BUILTIN_WORD(w_rad)
{
  const auto value = context.pop().as_number();

  context << (value * M_PI / 180L);
}

/**
 * number:>month ( number -- month )
 *
 * Converts number into a month assuming that January is 1 and December is 12
 * and so on.
 *
 * Range error will be thrown if the number is out of range.
 */
BUILTIN_WORD(w_to_month)
{
  const auto value = long(context.pop().as_number());

  if (value < 1 || value > 12)
  {
    throw error(error::type::range, U"Month index out of range.");
  }
  context << value::make_month(static_cast<peelo::chrono::month>(value - 1));
}

/**
 * number:>weekday ( number -- weekday )
 *
 * Converts number into day of week assuming that Sunday is 1 and Saturday is
 * 7 and so on.
 *
 * Range error will be thrown if the number is out of range.
 */
BUILTIN_WORD(w_to_weekday)
{
  const auto value = long(context.pop().as_number());

  if (value < 1 || value > 7)
  {
    throw error(error::type::range, U"Weekday index out of range.");
  }
  context << value::make_weekday(static_cast<peelo::chrono::weekday>(value - 1));
}

namespace laskin::api
{
  extern "C" const context::dictionary_definition number =
  {
    { U"number:has-unit?", w_has_unit },
    { U"number:unit", w_unit },
    { U"number:unit-type", w_unit_type },
    { U"number:drop-unit", w_drop_unit },

    { U"number:range", w_range },
    { U"number:clamp", w_clamp },
    { U"number:times", w_times },

    // Exponential functions.
    { U"number:exp", w_exp },
    { U"number:exp2", w_exp2 },
    { U"number:expm1", w_expm1 },
    { U"number:log", w_log },
    { U"number:log10", w_log10 },
    { U"number:log2", w_log2 },
    { U"number:log1p", w_log1p },

    // Power functions.
    { U"number:pow", w_pow },
    { U"number:sqrt", w_sqrt },
    { U"number:cbrt", w_cbrt },
    { U"number:hypot", w_hypot },

    // Trigonometric functions.
    { U"number:acos", w_acos },
    { U"number:asin", w_asin },
    { U"number:atan", w_atan },
    { U"number:atan2", w_atan2 },
    { U"number:cos", w_cos },
    { U"number:sin", w_sin },
    { U"number:tan", w_tan },
    { U"number:deg", w_deg },
    { U"number:rad", w_rad },

    // Hyperbolic functions.
    { U"number:sinh", w_sinh },
    { U"number:cosh", w_cosh },
    { U"number:tanh", w_tanh },
    { U"number:asinh", w_asinh },
    { U"number:acosh", w_acosh },
    { U"number:atanh", w_atanh },

    // Conversions.
    { U"number:>month", w_to_month },
    { U"number:>weekday", w_to_weekday }
  };
}
