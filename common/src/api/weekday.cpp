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
  static void w_sunday(class context& context, std::ostream&)
  {
    context << value::make_weekday(peelo::chrono::weekday::sun);
  }

  static void w_monday(class context& context, std::ostream&)
  {
    context << value::make_weekday(peelo::chrono::weekday::mon);
  }

  static void w_tuesday(class context& context, std::ostream&)
  {
    context << value::make_weekday(peelo::chrono::weekday::tue);
  }

  static void w_wednesday(class context& context, std::ostream&)
  {
    context << value::make_weekday(peelo::chrono::weekday::wed);
  }

  static void w_thursday(class context& context, std::ostream&)
  {
    context << value::make_weekday(peelo::chrono::weekday::thu);
  }

  static void w_friday(class context& context, std::ostream&)
  {
    context << value::make_weekday(peelo::chrono::weekday::fri);
  }

  static void w_saturday(class context& context, std::ostream&)
  {
    context << value::make_weekday(peelo::chrono::weekday::sat);
  }

  static void w_is_weekend(class context& context, std::ostream&)
  {
    const auto weekday = context.peek().as_weekday();

    context << value::make_boolean(
      weekday == peelo::chrono::weekday::sat
      || weekday == peelo::chrono::weekday::sun
    );
  }

  static void w_to_number(class context& context, std::ostream&)
  {
    const auto value = static_cast<int>(context.pop().as_weekday()) + 1;

    context << value::make_number(value);
  }

  namespace api
  {
    extern "C" const context::dictionary_definition weekday =
    {
      // Constants.
      { U"sunday", w_sunday },
      { U"monday", w_monday },
      { U"tuesday", w_tuesday },
      { U"wednesday", w_wednesday },
      { U"thursday", w_thursday },
      { U"friday", w_friday },
      { U"saturday", w_saturday },

      // Testing methods.
      { U"weekday:weekend?", w_is_weekend },

      // Conversions.
      { U"weekday:>number", w_to_number }
    };
  }
}
