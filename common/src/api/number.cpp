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
    const auto value = context.pop().as_number().value();

    context << value::make_number(value);
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

    if (count < number(0))
    {
      count = -count;
    }
    while (count)
    {
      --count;
      quote.call(context, out);
    }
  }

  static void double_op(class context& context, double (*callback)(double))
  {
    const auto value = context.pop().as_number();
    const auto result = callback(value.to_double());

    if (std::isnan(result))
    {
      throw error(error::type::domain);
    }
    context << value::make_number(result, value.measurement_unit());
  }

  static void w_acos(class context& context, std::ostream&)
  {
    double_op(context, std::acos);
  }

  static void w_asin(class context& context, std::ostream&)
  {
    double_op(context, std::asin);
  }

  static void w_atan(class context& context, std::ostream&)
  {
    double_op(context, std::atan);
  }

  static void w_cos(class context& context, std::ostream&)
  {
    double_op(context, std::cos);
  }

  static void w_sin(class context& context, std::ostream&)
  {
    double_op(context, std::sin);
  }

  static void w_tan(class context& context, std::ostream&)
  {
    double_op(context, std::tan);
  }

  static void w_deg(class context& context, std::ostream&)
  {
    const auto value = context.pop().as_number();

    context << value::make_number(
      value.value() * 180 / M_PI,
      value.measurement_unit()
    );
  }

  static void w_rad(class context& context, std::ostream&)
  {
    const auto value = context.pop().as_number();

    context << value::make_number(
      value.value() * M_PI / 180,
      value.measurement_unit()
    );
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

      // Trigonometry.
      { U"number:acos", w_acos },
      { U"number:asin", w_asin },
      { U"number:atan", w_atan },
      { U"number:cos", w_cos },
      { U"number:sin", w_sin },
      { U"number:tan", w_tan },
      { U"number:deg", w_deg },
      { U"number:rad", w_rad },

      // Conversions.
      { U"number:>month", w_to_month },
      { U"number:>weekday", w_to_weekday }
    };
  }
}
