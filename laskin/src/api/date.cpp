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
 * today ( -- date )
 *
 * Returns current date.
 */
LASKIN_BUILTIN_WORD(w_today)
{
  try
  {
    context << date::today();
  }
  catch (const std::runtime_error&)
  {
    throw laskin::error(
      laskin::error::type::system,
      U"System clock returned invalid date."
    );
  }
}

/**
 * tomorrow ( -- date )
 *
 * Returns tomorrow's date.
 */
LASKIN_BUILTIN_WORD(w_tomorrow)
{
  try
  {
    context << date::tomorrow();
  }
  catch (const std::runtime_error&)
  {
    throw laskin::error(
      laskin::error::type::system,
      U"System clock returned invalid date."
    );
  }
}

/**
 * yesterday ( -- date )
 *
 * Returns yesterday's date.
 */
LASKIN_BUILTIN_WORD(w_yesterday)
{
  try
  {
    context << date::yesterday();
  }
  catch (const std::runtime_error&)
  {
    throw laskin::error(
      laskin::error::type::system,
      U"System clock returned invalid date."
    );
  }
}

/**
 * date:year ( date -- date number )
 *
 * Extracts year from the date.
 */
LASKIN_BUILTIN_WORD(w_year)
{
  context << context.peek().as_date().year();
}

/**
 * date:month ( date -- date month )
 *
 * Extracts month from the date.
 */
LASKIN_BUILTIN_WORD(w_month)
{
  context << context.peek().as_date().month();
}

/**
 * date:day ( date -- date number )
 *
 * Extracts day of the month from the date.
 */
LASKIN_BUILTIN_WORD(w_day)
{
  context << context.peek().as_date().day();
}

/**
 * date:weekday ( date -- date weekday )
 *
 * Extracts day of the week from the date.
 */
LASKIN_BUILTIN_WORD(w_weekday)
{
  context << context.peek().as_date().day_of_week();
}

/**
 * date:day-of-year ( date -- date number )
 *
 * Extracts day of the year from the date.
 */
LASKIN_BUILTIN_WORD(w_day_of_year)
{
  context << context.peek().as_date().day_of_year();
}

/**
 * date:days-in-month ( date -- date number )
 *
 * Determines how many days there are in the month of the date.
 */
LASKIN_BUILTIN_WORD(w_days_in_month)
{
  context << context.peek().as_date().days_in_month();
}

/**
 * date:days-in-year ( date -- date number )
 *
 * Determines how many days there are in the year of the date.
 */
LASKIN_BUILTIN_WORD(w_days_in_year)
{
  context << context.peek().as_date().days_in_year();
}

/**
 * date:leap-year? ( date -- date boolean )
 *
 * Tests whether given date occurs on leap year.
 */
LASKIN_BUILTIN_WORD(w_is_leap_year)
{
  context << context.peek().as_date().is_leap_year();
}

/**
 * date:format ( string date -- string )
 *
 * Formats date with strftime().
 */
LASKIN_BUILTIN_WORD(w_format)
{
  using peelo::unicode::encoding::utf8::encode;
  using peelo::unicode::encoding::utf8::decode;

  const auto time = context.pop().as_date();
  const auto format = context.pop().as_string();

  context << decode(time.format(encode(format)));
}

/**
 * date:>number ( date -- number )
 *
 * Converts date into UNIX timestamp.
 */
LASKIN_BUILTIN_WORD(w_to_number)
{
  context << number(
    context.pop().as_date().timestamp(),
    number::unit::second
  );
}

/**
 * date:>vector ( date -- vector )
 *
 * Extracts year, month and day of month from the date and returns them
 * inside an vector.
 */
LASKIN_BUILTIN_WORD(w_to_vector)
{
  const auto date = context.pop().as_date();

  context << vector{ date.year(), date.month(), date.day() };
}

namespace laskin::api
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
