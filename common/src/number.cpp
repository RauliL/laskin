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
#include <utility>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/error.hpp"
#include "laskin/number.hpp"

namespace laskin
{
  static number::value_type to_base_unit(const number&);
  template<class Operator>
  static number binary_op(const number&, const number&);
  static void unit_check(const number&, const number&);

  bool number::is_valid(const std::u32string& input)
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

  number number::parse(const std::u32string& input)
  {
    const auto length = input.length();
    std::u32string::size_type start;
    bool dot_seen = false;

    if (!length)
    {
      return number();
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
          value_type(
            peelo::unicode::encoding::utf8::encode(input.substr(0, i))
          ),
          unit::find_by_symbol(input.substr(i, length - i))
        );
      }
    }

    return number(
      value_type(peelo::unicode::encoding::utf8::encode(input)),
      unit_type()
    );
  }

  number::number(const value_type& value, const unit_type& measurement_unit)
    : m_value(value)
    , m_measurement_unit(measurement_unit) {}

  number::number(const number& that)
    : m_value(that.m_value)
    , m_measurement_unit(that.m_measurement_unit) {}

  number::number(number&& that)
    : m_value(std::move(that.m_value))
    , m_measurement_unit(std::move(that.m_measurement_unit)) {}

  long number::to_long() const
  {
    if (!m_value.fits_slong_p())
    {
      throw error(error::type::range, U"Numeric value is too large.");
    }

    return m_value.get_si();
  }

  double number::to_double() const
  {
    return m_value.get_d();
  }

  number& number::operator=(const number& that)
  {
    m_value = that.m_value;
    m_measurement_unit = that.m_measurement_unit;

    return *this;
  }

  number& number::operator=(number&& that)
  {
    m_value = std::move(that.m_value);
    m_measurement_unit = std::move(that.m_measurement_unit);

    return *this;
  }

  bool number::operator==(const number& that) const
  {
    unit_check(*this, that);

    return to_base_unit(*this) == to_base_unit(that);
  }

  bool number::operator!=(const number& that) const
  {
    unit_check(*this, that);

    return to_base_unit(*this) != to_base_unit(that);
  }

  bool number::operator<(const number& that) const
  {
    unit_check(*this, that);

    return to_base_unit(*this) < to_base_unit(that);
  }

  bool number::operator>(const number& that) const
  {
    unit_check(*this, that);

    return to_base_unit(*this) > to_base_unit(that);
  }

  bool number::operator<=(const number& that) const
  {
    unit_check(*this, that);

    return to_base_unit(*this) <= to_base_unit(that);
  }

  bool number::operator>=(const number& that) const
  {
    unit_check(*this, that);

    return to_base_unit(*this) >= to_base_unit(that);
  }

  number number::operator+(const number& that) const
  {
    return binary_op<std::plus<value_type>>(*this, that);
  }

  number number::operator-(const number& that) const
  {
    return binary_op<std::minus<value_type>>(*this, that);
  }

  number number::operator*(const number& that) const
  {
    return binary_op<std::multiplies<value_type>>(*this, that);
  }

  number number::operator/(const number& that) const
  {
    if (that.m_value == 0)
    {
      throw error(error::type::range, U"Division by zero.");
    }

    return binary_op<std::divides<value_type>>(*this, that);
  }

  number& number::operator++()
  {
    ++m_value;

    return *this;
  }

  number& number::operator--()
  {
    --m_value;

    return *this;
  }

  number number::operator++(int)
  {
    const auto copy(*this);

    ++m_value;

    return copy;
  }

  number number::operator--(int)
  {
    const auto copy(*this);

    --m_value;

    return copy;
  }

  number number::operator-() const
  {
    return number(-m_value);
  }

  std::ostream& operator<<(std::ostream& out, const number& num)
  {
    const auto& value = num.value();
    ::mp_exp_t exp = 0;
    auto buffer = ::mpf_get_str(
      nullptr,
      &exp,
      10,
      0,
      value.get_mpf_t()
    );
    std::string result(buffer);

    if (buffer)
    {
      std::free(buffer);
    }
    if (result.empty())
    {
      result.assign(1, '0');
    }
    if (value < 0)
    {
      ++exp;
    }
    if (exp > 0)
    {
      if (static_cast<std::string::size_type>(exp) >= result.length())
      {
        const auto size = result.length();

        for (
          std::string::size_type i = 0;
          i < static_cast<std::string::size_type>(exp) - size;
          ++i
        )
        {
          result.append(1, '0');
        }
      } else {
        result.insert(std::begin(result) + exp, '.');
      }
    } else {
      result.insert(0, "0.");
    }
    if (const auto& unit = num.measurement_unit())
    {
      result += peelo::unicode::encoding::utf8::encode(unit->symbol());
    }
    out << result;

    return out;
  }

  /**
   * Determines base unit of the given number, if the given number has a
   * measurement unit.
   */
  static number::unit_type base_unit_of(const number& num)
  {
    const auto& unit = num.measurement_unit();

    return unit ? unit::base_unit_of(unit->type()) : number::unit_type();
  }

  static number::value_type to_base_unit(const number& num)
  {
    const auto& value = num.value();
    const auto& unit = num.measurement_unit();

    if (unit && !unit->is_base_unit())
    {
      const auto multiplier = unit->multiplier();

      if (multiplier > 0)
      {
        return value * multiplier;
      }
      else if (multiplier < 0)
      {
        return value / -multiplier;
      }
    }

    return value;
  }

  template<class Operator>
  static number binary_op(const number& a, const number& b)
  {
    const auto base = base_unit_of(a);
    const auto base_value_a = to_base_unit(a);
    const auto base_value_b = to_base_unit(b);
    number::value_type result;

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

  static void unit_check(const number& a, const number& b)
  {
    const auto& unit_a = a.measurement_unit();
    const auto& unit_b = b.measurement_unit();

    if (unit_a && unit_b)
    {
      const auto type_a = unit_a->type();
      const auto type_b = unit_b->type();

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
    else if (unit_b)
    {
      throw error(
        error::type::unit,
        U"Cannot compare number without an unit against number which has " +
        to_string(unit_b->type()) +
        U" as measurement unit."
      );
    }
  }
}
