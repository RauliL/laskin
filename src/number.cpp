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
#include <sstream>
#include <unordered_map>
#include <vector>

#include <peelo/unicode.hpp>

#include "laskin/error.hpp"
#include "laskin/number.hpp"

namespace laskin
{
  static void unit_check(const number*, const number&);
  static void convert_to_base_unit(mpf_class&, unit&);

  number::number(const value_type& value, unit_type unit)
    : m_value(value)
    , m_unit(unit) {}

  number::number(const number& that)
    : m_value(that.m_value)
    , m_unit(that.m_unit) {}

  bool number::isnumber(const std::u32string& input)
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
        return find_unit_by_name(input.substr(i, length - i)) != unit::none;
      }
    }

    return true;
  }

  number number::from_string(const std::u32string& input)
  {
    const auto length = input.length();
    std::u32string::size_type start;
    bool dot_seen = false;

    if (!length)
    {
      return number(mpf_class());
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
        mpf_class value(peelo::unicode::utf8::encode(input.substr(0, i)));
        auto unit = find_unit_by_name(input.substr(i, length - i));

        convert_to_base_unit(value, unit);

        return number(value, unit);
      }
    }

    return number(mpf_class(peelo::unicode::utf8::encode(input)));
  }

  long number::as_long() const
  {
    if (!m_value.fits_slong_p())
    {
      throw error(error::type_range, U"Numeric value is too large.");
    }

    return m_value.get_si();
  }

  std::u32string number::to_string() const
  {
    std::stringstream ss;
    std::u32string decoded;

    ss << m_value;
    decoded = peelo::unicode::utf8::decode(ss.str());
    if (m_unit != unit_type::none)
    {
      decoded += unit_name(m_unit);
    }

    return decoded;
  }

  number number::operator+(const number& that) const
  {
    unit_check(this, that);

    return number(m_value + that.m_value, m_unit);
  }

  number number::operator-(const number& that) const
  {
    unit_check(this, that);

    return number(m_value - that.m_value, m_unit);
  }

  number number::operator*(const number& that) const
  {
    unit_check(this, that);

    return number(m_value * that.m_value, m_unit);
  }

  number number::operator/(const number& that) const
  {
    unit_check(this, that);
    if (that.m_value == 0.0)
    {
      throw error(error::type_range, U"Division by zero.");
    }

    return number(m_value / that.m_value, m_unit);
  }

  number number::operator+(int operand) const
  {
    return number(m_value + operand, m_unit);
  }

  number number::operator-(int operand) const
  {
    return number(m_value - operand, m_unit);
  }

  number number::operator*(int operand) const
  {
    return number(m_value * operand, m_unit);
  }

  number number::operator/(int operand) const
  {
    if (operand == 0)
    {
      throw error(error::type_range, U"Division by zero.");
    }

    return number(m_value / operand, m_unit);
  }

  number& number::operator+=(const number& that)
  {
    unit_check(this, that);
    m_value += that.m_value;

    return *this;
  }

  number& number::operator-=(const number& that)
  {
    unit_check(this, that);
    m_value -= that.m_value;

    return *this;
  }

  number& number::operator*=(const number& that)
  {
    unit_check(this, that);
    m_value *= that.m_value;

    return *this;
  }

  number& number::operator/=(const number& that)
  {
    unit_check(this, that);
    if (that.m_value == 0.0)
    {
      throw error(error::type_range, U"Division by zero.");
    }
    m_value /= that.m_value;

    return *this;
  }

  number& number::operator+=(int operand)
  {
    m_value += operand;

    return *this;
  }

  number& number::operator-=(int operand)
  {
    m_value -= operand;

    return *this;
  }

  number& number::operator*=(int operand)
  {
    m_value *= operand;

    return *this;
  }

  number& number::operator/=(int operand)
  {
    if (operand == 0)
    {
      throw error(error::type_range, U"Division by zero.");
    }
    m_value /= operand;

    return *this;
  }

  bool number::operator==(const number& that) const
  {
    unit_check(this, that);

    return m_value == that.m_value;
  }

  bool number::operator!=(const number& that) const
  {
    unit_check(this, that);

    return m_value != that.m_value;
  }

  bool number::operator<(const number& that) const
  {
    unit_check(this, that);

    return m_value < that.m_value;
  }

  bool number::operator>(const number& that) const
  {
    unit_check(this, that);

    return m_value > that.m_value;
  }

  bool number::operator<=(const number& that) const
  {
    unit_check(this, that);

    return m_value <= that.m_value;
  }

  bool number::operator>=(const number& that) const
  {
    unit_check(this, that);

    return m_value >= that.m_value;
  }

  std::ostream& operator<<(std::ostream& out, const class number& number)
  {
    out << peelo::unicode::utf8::encode(number.to_string());

    return out;
  }

  static void unit_check(const number* a, const number& b)
  {
    const auto quantity_a = unit_quantity(a->unit());
    const auto quantity_b = unit_quantity(b.unit());

    if (quantity_a != quantity_b)
    {
      throw error(
        error::type_unit,
        U"Cannot compare " +
        quantity_name(quantity_a) +
        U" against " +
        quantity_name(quantity_b) +
        U"."
      );
    }
  }

  static void convert_to_base_unit(mpf_class& value, unit& u)
  {
    switch (u)
    {
      // Length units.
      case unit::mm:
        u = unit::m;
        value /= 1000;
        break;

      case unit::cm:
        u = unit::m;
        value /= 100;
        break;

      case unit::km:
        u = unit::m;
        value *= 1000;
        break;

      // Mass units.
      case unit::mg:
        u = unit::kg;
        value /= 1000000;
        break;

      case unit::g:
        u = unit::kg;
        value /= 1000;
        break;

      // Time units.
      case unit::ms:
        u = unit::s;
        value /= 1000;
        break;

      case unit::min:
        u = unit::s;
        value *= 60;
        break;

      case unit::h:
        u = unit::s;
        value *= 3600;
        break;

      case unit::d:
        u = unit::s;
        value *= 86400;
        break;

      default:
        break;
    }
  }
}
