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

  namespace api
  {
    extern "C" const context::dictionary_definition number =
    {
      { U"number:range", w_range },
      { U"number:clamp", w_clamp },

      // Trigonometry.
      { U"number:acos", w_acos },
      { U"number:asin", w_asin },
      { U"number:atan", w_atan },
      { U"number:cos", w_cos },
      { U"number:sin", w_sin },
      { U"number:tan", w_tan },
      { U"number:deg", w_deg },
      { U"number:rad", w_rad }
    };
  }
}
