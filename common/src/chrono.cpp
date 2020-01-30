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
#include <climits>

#include "laskin/chrono.hpp"
#include "laskin/error.hpp"

namespace laskin
{
  static inline bool is_digits(
    const std::u32string&,
    const std::u32string::size_type,
    const std::u32string::size_type
  );
  static int to_integer(
    const std::u32string&,
    const std::u32string::size_type,
    const std::u32string::size_type
  );

  bool is_date(const std::u32string& input)
  {
    const auto length = input.length();
    std::u32string::size_type dash1;
    std::u32string::size_type dash2;

    return (
      length > 4
      && (dash1 = input.find('-')) != std::u32string::npos
      && (dash2 = input.find('-', dash1 + 1)) != std::u32string::npos
      && is_digits(input, 0, dash1)
      && is_digits(input, dash1 + 1, dash2)
      && is_digits(input, dash2 + 1, length)
    );
  }

  bool is_time(const std::u32string& input)
  {
    return (
      input.length() == 8
      && input[2] == ':'
      && input[5] == ':'
      && is_digits(input, 0, 2)
      && is_digits(input, 3, 5)
      && is_digits(input, 6, 8)
    );
  }

  peelo::chrono::date parse_date(const std::u32string& input)
  {
    const auto length = input.length();
    std::u32string::size_type dash1;
    std::u32string::size_type dash2;
    int year;
    int month;
    int day;
    peelo::chrono::month converted_month;

    if (length < 5
        || (dash1 = input.find('-')) == std::u32string::npos
        || (dash2 = input.find('-', dash1 + 1)) == std::u32string::npos
        || !is_digits(input, 0, dash1)
        || !is_digits(input, dash1 + 1, dash2)
        || !is_digits(input, dash2 + 1, length))
    {
      throw error(
        error::type::syntax,
        U"Given date literal does not contain valid date."
      );
    }

    year = to_integer(input, 0, dash1);
    month = to_integer(input, dash1 + 1, dash2);
    day = to_integer(input, dash2 + 1, length);

    if (month < 1 || month > 12)
    {
      throw error(error::type::range, U"Given month is out of range.");
    }
    converted_month = static_cast<peelo::chrono::month>(month - 1);

    if (!peelo::chrono::date::is_valid(year, converted_month, day))
    {
      throw error(error::type::range, U"Given date literal is out of range.");
    }

    return peelo::chrono::date(year, converted_month, day);
  }

  peelo::chrono::time parse_time(const std::u32string& input)
  {
    int hour;
    int minute;
    int second;

    if (input.length() != 8
        || input[2] != ':'
        || input[5] != ':'
        || !is_digits(input, 0, 2)
        || !is_digits(input, 3, 5)
        || !is_digits(input, 6, 8))
    {
      throw error(
        error::type::syntax,
        U"Given time literal does not contain valid time."
      );
    }

    hour = to_integer(input, 0, 2);
    minute = to_integer(input, 3, 5);
    second = to_integer(input, 6, 8);

    if (!peelo::chrono::time::is_valid(hour, minute, second))
    {
      throw error(error::type::range, U"Given time value is out of range.");
    }

    return peelo::chrono::time(hour, minute, second);
  }

  static bool is_digits(const std::u32string& input,
                        const std::u32string::size_type from,
                        const std::u32string::size_type to)
  {
    if (to - from == 0)
    {
      return false;
    }
    for (auto i = from; i < to; ++i)
    {
      if (!std::isdigit(input[i]))
      {
        return false;
      }
    }

    return true;
  }

  static int to_integer(const std::u32string& input,
                        const std::u32string::size_type from,
                        const std::u32string::size_type to)
  {
    static const auto div = INT_MAX / 10;
    static const auto rem = INT_MAX % 10;
    int result = 0;

    for (auto i = from; i < to; ++i)
    {
      const auto c = input[i];
      int digit;

      if (!std::isdigit(c))
      {
        continue;
      }
      digit = c - '0';
      if (result > div || (result == div && digit > rem))
      {
        // Integer underflow / overflow.
        continue;
      }
      result = (result * 10) + digit;
    }

    return result;
  }
}
