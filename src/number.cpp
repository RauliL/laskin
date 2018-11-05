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
#include <peelo/unicode.hpp>

#include "laskin/error.hpp"
#include "laskin/number.hpp"

namespace laskin
{
  static std::optional<unit> base_unit_of(const number&);
  static mpf_class to_base_unit(const number&);
  static void unit_check(const number&, const number&);

  bool is_number(const std::u32string& input)
  {
    const auto length = input.length();
    std::u32string::size_type start;
    bool dot_seen = false;

    if (!length)
    {
      return false;
    }
    if (input[0] == '+' || input[0] == '-')
    {
      start = 1;
      if (length < 2)
      {
        return false;
      }
    } else {
      start = 0;
    }
    for (std::u32string::size_type i = start; i < length; ++i)
    {
      const auto& c = input[i];

      if (c == '.')
      {
        if (dot_seen || i == start || i + 1 > length)
        {
          return false;
        }
        dot_seen = true;
      }
      else if (!std::isdigit(c))
      {
        return unit::find_by_symbol(input.substr(i, length - i)).has_value();
      }
    }

    return true;
  }

  number parse_number(const std::u32string& input)
  {
    const auto length = input.length();
    std::u32string::size_type start;
    bool dot_seen = false;

    if (!length)
    {
      return number(mpf_class(), std::optional<unit>());
    }
    if (input[0] == '+' || input[0] == '-')
    {
      start = 1;
    } else {
      start = 0;
    }
    for (std::u32string::size_type i = start; i < length; ++i)
    {
      const auto& c = input[i];

      if (c == '.')
      {
        if (dot_seen || i == start || i + 1 > length)
        {
          break;
        }
        dot_seen = true;
      }
      else if (!std::isdigit(c))
      {
        return number(
          mpf_class(peelo::unicode::utf8::encode(input.substr(0, i))),
          unit::find_by_symbol(input.substr(i, length - i))
        );
      }
    }

    return number(
      mpf_class(peelo::unicode::utf8::encode(input)),
      std::optional<unit>()
    );
  }

  long to_long(const number& num)
  {
    if (!num.first.fits_slong_p())
    {
      throw error(error::type::range, U"Numeric value is too large.");
    }

    return num.first.get_si();
  }

  double to_double(const number& num)
  {
    return num.first.get_d();
  }

  template<class Operator>
  static number binary_op(const number& a, const number& b)
  {
    const auto base = base_unit_of(a);
    const auto base_value_a = to_base_unit(a);
    const auto base_value_b = to_base_unit(b);
    mpf_class result;

    unit_check(a, b);
    result = Operator()(base_value_a, base_value_b);
    if (base)
    {
      for (const auto& unit : unit::all_units_of(base->type()))
      {
        const auto multiplier = unit.multiplier();

        if (multiplier > 0 && result >= multiplier)
        {
          return number(result / multiplier, unit);
        }
      }
    }

    return number(result, base);
  }

  number operator+(const number& a, const number& b)
  {
    return binary_op<std::plus<mpf_class>>(a, b);
  }

  number operator-(const number& a, const number& b)
  {
    return binary_op<std::minus<mpf_class>>(a, b);
  }

  number operator*(const number& a, const number& b)
  {
    return binary_op<std::multiplies<mpf_class>>(a, b);
  }

  number operator/(const number& a, const number& b)
  {
    if (b.first == 0)
    {
      throw error(error::type::range, U"Division by zero.");
    }

    return binary_op<std::divides<mpf_class>>(a, b);
  }

  number& operator++(number& num)
  {
    ++num.first;

    return num;
  }

  number& operator--(number& num)
  {
    --num.first;

    return num;
  }

  bool operator==(const number& a, const number& b)
  {
    unit_check(a, b);

    return to_base_unit(a) == to_base_unit(b);
  }

  bool operator!=(const number& a, const number& b)
  {
    unit_check(a, b);

    return to_base_unit(a) != to_base_unit(b);
  }

  bool operator<(const number& a, const number& b)
  {
    unit_check(a, b);

    return to_base_unit(a) < to_base_unit(b);
  }

  bool operator>(const number& a, const number& b)
  {
    unit_check(a, b);

    return to_base_unit(a) > to_base_unit(b);
  }

  bool operator<=(const number& a, const number& b)
  {
    unit_check(a, b);

    return to_base_unit(a) <= to_base_unit(b);
  }

  bool operator>=(const number& a, const number& b)
  {
    unit_check(a, b);

    return to_base_unit(a) >= to_base_unit(b);
  }

  std::ostream& operator<<(std::ostream& out, const number& num)
  {
    out << num.first;
    if (num.second)
    {
      out << peelo::unicode::utf8::encode(num.second->symbol());
    }

    return out;
  }

  /**
   * Determines base unit of the given number, if the given number has a
   * measurement unit.
   */
  static std::optional<unit> base_unit_of(const number& num)
  {
    if (num.second)
    {
      return unit::base_unit_of(num.second->type());
    }

    return std::optional<unit>();
  }

  static mpf_class to_base_unit(const number& num)
  {
    if (num.second && !num.second->is_base_unit())
    {
      const auto multiplier = num.second->multiplier();

      if (multiplier > 0)
      {
        return num.first * multiplier;
      }
      else if (multiplier < 0)
      {
        return num.first / -multiplier;
      }
    }

    return num.first;
  }

  static void unit_check(const number& a, const number& b)
  {
    if (a.second && b.second)
    {
      const auto type_a = a.second->type();
      const auto type_b = b.second->type();

      if (type_a != type_b)
      {
        throw error(
          error::type::unit,
          U"Cannot compare " +
          to_string(type_a) +
          U" against " +
          to_string(type_b) +
          U"."
        );
      }
    }
    else if (b.second)
    {
      throw error(
        error::type::unit,
        U"Cannot compare number without an unit against number which has " +
        to_string(b.second->type()) +
        U" as measurement unit."
      );
    }
  }
}
