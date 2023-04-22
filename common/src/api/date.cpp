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
#include <chrono>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"

namespace laskin
{
  static inline peelo::chrono::date get_current_date()
  {
    const auto now = std::chrono::system_clock::now();
    const auto ts = std::chrono::system_clock::to_time_t(now);
    const auto tm = std::localtime(&ts);

    try
    {
      return peelo::chrono::date(
        tm->tm_year + 1900,
        static_cast<enum peelo::chrono::month>(tm->tm_mon),
        tm->tm_mday
      );
    }
    catch (const std::invalid_argument&)
    {
      throw laskin::error(
        laskin::error::type::system,
        U"System clock returned invalid date."
      );
    }
  }

  static void w_today(class context& context, std::ostream&)
  {
    context << value::make_date(get_current_date());
  }

  static void w_tomorrow(class context& context, std::ostream&)
  {
    context << value::make_date(get_current_date() + 1);
  }

  static void w_yesterday(class context& context, std::ostream&)
  {
    context << value::make_date(get_current_date() - 1);
  }

  static void w_year(class context& context, std::ostream&)
  {
    context << value::make_number(context.peek().as_date().year());
  }

  static void w_month(class context& context, std::ostream&)
  {
    context << value::make_month(context.peek().as_date().month());
  }

  static void w_day(class context& context, std::ostream&)
  {
    context << value::make_number(context.peek().as_date().day());
  }

  static void w_weekday(class context& context, std::ostream&)
  {
    context << value::make_weekday(context.peek().as_date().day_of_week());
  }

  static void w_day_of_year(class context& context, std::ostream&)
  {
    context << value::make_number(context.peek().as_date().day_of_year());
  }

  static void w_days_in_month(class context& context, std::ostream&)
  {
    context << value::make_number(context.peek().as_date().days_in_month());
  }

  static void w_days_in_year(class context& context, std::ostream&)
  {
    context << value::make_number(context.peek().as_date().days_in_year());
  }

  static void w_is_leap_year(class context& context, std::ostream&)
  {
    context << value::make_boolean(context.peek().as_date().is_leap_year());
  }

  static void w_format(class context& context, std::ostream&)
  {
    using peelo::unicode::encoding::utf8::encode;
    using peelo::unicode::encoding::utf8::decode;
    const auto time = context.pop().as_date();
    const auto format = context.pop().as_string();

    context << value::make_string(decode(time.format(encode(format))));
  }

  static void w_to_number(class context& context, std::ostream&)
  {
    context << value::make_number(
      context.pop().as_date().timestamp(),
      unit::second
    );
  }

  static void w_to_vector(class context& context, std::ostream&)
  {
    const auto date = context.pop().as_date();
    const auto year = date.year();
    const auto month = date.month();
    const auto day = date.day();

    context << value::make_vector({
      value::make_number(year),
      value::make_month(month),
      value::make_number(day)
    });
  }

  namespace api
  {
    extern "C" const context::dictionary_definition date =
    {
      { U"today", w_today },
      { U"tomorrow", w_tomorrow },
      { U"yesterday", w_yesterday },

      { U"date:year", w_year },
      { U"date:month", w_month },
      { U"date:day", w_day },
      { U"date:weekday", w_weekday },
      { U"date:day-of-year", w_day_of_year },
      { U"date:days-in-month", w_days_in_month },
      { U"date:days-in-year", w_days_in_year },
      { U"date:leap-year?", w_is_leap_year },

      { U"date:format", w_format },

      { U"date:>number", w_to_number },
      { U"date:>vector", w_to_vector }
    };
  }
}
