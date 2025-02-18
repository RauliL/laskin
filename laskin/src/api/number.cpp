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

#include "laskin/context.hpp"
#include "laskin/error.hpp"

namespace laskin
{
  static void w_has_unit(class context& context, std::ostream&)
  {
    context << value::make_boolean(bool(context.peek().as_number().measurement_unit()));
  }

  static void w_unit(class context& context, std::ostream&)
  {
    const auto& unit = context.peek().as_number().measurement_unit();

    if (!unit)
    {
      throw error(error::type::unit, U"Value has no measurement unit.");
    }
    context << value::make_string(unit->symbol());
  }

  static void w_unit_type(class context& context, std::ostream&)
  {
    const auto& unit = context.peek().as_number().measurement_unit();

    if (!unit)
    {
      throw error(error::type::unit, U"Value has no measurement unit.");
    }
    context << value::make_string(to_string(unit->type()));
  }

  static void w_drop_unit(class context& context, std::ostream&)
  {
    const auto value = context.pop().as_number();

    context << value::make_number(value.without_unit());
  }

  static void w_range(class context& context, std::ostream&)
  {
    const auto limit = context.pop().as_number();
    auto current = context.pop().as_number();
    std::vector<value> result;

    while (current < limit)
    {
      result.push_back(value::make_number(current));
      ++current;
    }
    context << value::make_vector(result);
  }

  static void w_clamp(class context& context, std::ostream&)
  {
    const auto value = context.pop().as_number();
    const auto max = context.pop().as_number();
    const auto min = context.pop().as_number();

    context.push(value::make_number(
      value > max ? max : value < min ? min : value
    ));
  }

  static void w_times(class context& context, std::ostream& out)
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

  static void w_exp(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().exp());
  }

  static void w_exp2(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().exp2());
  }

  static void w_expm1(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().expm1());
  }

  static void w_log(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().log());
  }

  static void w_log10(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().log10());
  }

  static void w_log2(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().log2());
  }

  static void w_log1p(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().log1p());
  }

  static void w_pow(class context& context, std::ostream&)
  {
    const auto a = context.pop().as_number();
    const auto b = context.pop().as_number();

    context << value::make_number(a.pow(b));
  }

  static void w_sqrt(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().sqrt());
  }

  static void w_cbrt(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().cbrt());
  }

  static void w_hypot(class context& context, std::ostream&)
  {
    const auto a = context.pop().as_number();
    const auto b = context.pop().as_number();

    context << value::make_number(a.hypot(b));
  }

  static void w_acos(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().acos());
  }

  static void w_asin(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().asin());
  }

  static void w_atan(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().atan());
  }

  static void w_atan2(class context& context, std::ostream&)
  {
    const auto a = context.pop().as_number();
    const auto b = context.pop().as_number();

    context << value::make_number(a.atan2(b));
  }

  static void w_cos(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().cos());
  }

  static void w_sin(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().sin());
  }

  static void w_tan(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().tan());
  }

  static void w_sinh(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().sinh());
  }

  static void w_cosh(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().cosh());
  }

  static void w_tanh(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().tanh());
  }

  static void w_asinh(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().asinh());
  }

  static void w_acosh(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().acosh());
  }

  static void w_atanh(class context& context, std::ostream&)
  {
    context << value::make_number(context.pop().as_number().atanh());
  }

  static void w_deg(class context& context, std::ostream&)
  {
    const auto value = context.pop().as_number();

    context << value::make_number(value * 180 / M_PI);
  }

  static void w_rad(class context& context, std::ostream&)
  {
    const auto value = context.pop().as_number();

    context << value::make_number(value * M_PI / 180L);
  }

  static void w_to_month(class context& context, std::ostream&)
  {
    const auto value = context.pop().as_number().to_long();

    if (value < 1 || value > 12)
    {
      throw error(error::type::range, U"Month index out of range.");
    }
    context << value::make_month(static_cast<peelo::chrono::month>(value - 1));
  }

  static void w_to_weekday(class context& context, std::ostream&)
  {
    const auto value = context.pop().as_number().to_long();

    if (value < 1 || value > 7)
    {
      throw error(error::type::range, U"Weekday index out of range.");
    }
    context << value::make_weekday(static_cast<peelo::chrono::weekday>(value - 1));
  }

  namespace api
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
}
