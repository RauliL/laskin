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
#include <functional>

#include <peelo/unicode.hpp>

#include "laskin/error.hpp"
#include "laskin/number.hpp"

namespace laskin
{
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
        return unit::find_by_name(input.substr(i, length - i)).has_value();
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
      return number(mpf_class(), unit::optional_any());
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
          unit::find_by_name(input.substr(i, length - i))
        );
      }
    }

    return number(
      mpf_class(peelo::unicode::utf8::encode(input)),
      unit::optional_any()
    );
  }

  long to_long(const number& num)
  {
    if (!num.first.fits_slong_p())
    {
      throw error(error::type_range, U"Numeric value is too large.");
    }

    return num.first.get_si();
  }

  static void unit_check(const number& a, const number& b)
  {
    const auto quantity_a = unit::quantity_of(a.second);
    const auto quantity_b = unit::quantity_of(b.second);

    if (quantity_a.compare(quantity_b))
    {
      throw error(
        error::type_unit,
        U"Cannot compare " +
        quantity_a +
        U" against " +
        quantity_b +
        U"."
      );
    }
  }

  static mpf_class to_base_unit(const number& num)
  {
    unit::any unit;

    if (!num.second)
    {
      return num.first;
    }
    unit = num.second.value();
    if (std::holds_alternative<unit::length>(unit))
    {
      switch (std::get<unit::length>(unit))
      {
        case unit::length::mm:
          return num.first / 1000;

        case unit::length::cm:
          return num.first / 100;

        case unit::length::km:
          return num.first * 1000;

        default:
          return num.first;
      }
    }
    else if (std::holds_alternative<unit::mass>(unit))
    {
      switch (std::get<unit::mass>(unit))
      {
        case unit::mass::mg:
          return num.first / 1000000;

        case unit::mass::g:
          return num.first / 1000;

        default:
          return num.first;
      }
    }
    else if (std::holds_alternative<unit::time>(unit))
    {
      switch (std::get<unit::time>(unit))
      {
        case unit::time::ms:
          return num.first / 1000;

        case unit::time::min:
          return num.first * 60;

        case unit::time::h:
          return num.first * 3600;

        case unit::time::d:
          return num.first * 86400;

        default:
          return num.first;
      }
    }

    return num.first;
  }

  template<class Operator>
  static number binary_op(const number& a, const number& b)
  {
    auto base = unit::base_unit_of(a.second);
    auto base_a = to_base_unit(a);
    auto base_b = to_base_unit(b);
    mpf_class result;

    unit_check(a, b);
    result = Operator()(base_a, base_b);

    if (base)
    {
      auto base_value = base.value();

      if (std::holds_alternative<unit::length>(base_value))
      {
        if (result >= 1000)
        {
          result /= 1000;
          base = unit::length::km;
        }
      }
      else if (std::holds_alternative<unit::time>(base_value))
      {
        if (result >= 86400)
        {
          result /= 86400;
          base = unit::time::d;
        }
        else if (result >= 3600)
        {
          result /= 3600;
          base = unit::time::h;
        }
        else if (result >= 60)
        {
          result /= 60;
          base = unit::time::min;
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
      throw error(error::type_range, U"Division by zero.");
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
      out << peelo::unicode::utf8::encode(unit::name_of(num.second.value()));
    }

    return out;
  }
}
