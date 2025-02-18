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
#include <cstdint>
#include <functional>
#include <sstream>
#include <utility>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/error.hpp"
#include "laskin/number.hpp"

namespace laskin
{
  static constexpr mpfr_rnd_t default_rounding = MPFR_RNDN;

  using binary_op_callback = int (*)(
    mpfr_t,
    mpfr_srcptr,
    mpfr_srcptr,
    mpfr_rnd_t
  );

  static void to_base_unit(number::value_type, const number&);
  static void binary_op(
    const number&,
    const number&,
    number::value_type,
    number::unit_type&,
    binary_op_callback
  );
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
    number result;

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
        mpfr_init_set_str(
          result.m_value,
          peelo::unicode::encoding::utf8::encode(input.substr(0, i)).c_str(),
          10,
          default_rounding
        );
        result.m_measurement_unit = unit::find_by_symbol(
          input.substr(i, length - i)
        );

        return result;
      }
    }
    mpfr_init_set_str(
      result.m_value,
      peelo::unicode::encoding::utf8::encode(input).c_str(),
      10,
      default_rounding
    );

    return result;
  }

  number::number(const unit_type& measurement_unit)
    : m_measurement_unit(measurement_unit)
  {
    mpfr_init_set_ui(m_value, 0, default_rounding);
  }

  number::number(std::int64_t value, const unit_type& measurement_unit)
    : m_measurement_unit(measurement_unit)
  {
    mpfr_init_set_si(m_value, value, default_rounding);
  }

  number::number(double value, const unit_type& measurement_unit)
    : m_measurement_unit(measurement_unit)
  {
    mpfr_init_set_d(m_value, value, default_rounding);
  }

  number::number(const number& that)
    : m_measurement_unit(that.m_measurement_unit)
  {
    mpfr_init_set(m_value, that.m_value, default_rounding);
  }

  number::~number()
  {
    mpfr_clear(m_value);
  }

  long number::to_long() const
  {
    if (!mpfr_fits_slong_p(m_value, default_rounding))
    {
      throw error(error::type::range, U"Numeric value is too large.");
    }

    return mpfr_get_si(m_value, default_rounding);
  }

  double number::to_double() const
  {
    return mpfr_get_d(m_value, default_rounding);
  }

  number number::without_unit() const
  {
    number result(*this);

    if (result.m_measurement_unit)
    {
      result.m_measurement_unit.reset();
    }

    return result;
  }

  number& number::operator=(const number& that)
  {
    mpfr_set(m_value, that.m_value, default_rounding);
    m_measurement_unit = that.m_measurement_unit;

    return *this;
  }

  int number::compare(const number& that) const
  {
    value_type a;
    value_type b;
    int result;

    unit_check(*this, that);
    to_base_unit(a, *this);
    to_base_unit(b, that);
    result = mpfr_cmp(a, b);
    mpfr_clear(a);
    mpfr_clear(b);

    return result;
  }

  number number::operator+(const number& that) const
  {
    number result;

    binary_op(
      *this,
      that,
      result.m_value,
      result.m_measurement_unit,
      mpfr_add
    );

    return result;
  }

  number number::operator-(const number& that) const
  {
    number result;

    binary_op(
      *this,
      that,
      result.m_value,
      result.m_measurement_unit,
      mpfr_sub
    );

    return result;
  }

  number number::operator*(const number& that) const
  {
    number result;

    binary_op(
      *this,
      that,
      result.m_value,
      result.m_measurement_unit,
      mpfr_mul
    );

    return result;
  }

  number number::operator*(std::int64_t that) const
  {
    number result(m_measurement_unit);

    mpfr_mul_si(result.m_value, m_value, that, default_rounding);

    return result;
  }

  number number::operator/(const number& that) const
  {
    number result;

    if (mpfr_cmp_si(that.m_value, 0) == 0)
    {
      throw error(error::type::range, U"Division by zero.");
    }
    binary_op(
      *this,
      that,
      result.m_value,
      result.m_measurement_unit,
      mpfr_div
    );

    return result;
  }

  number number::operator/(std::int64_t that) const
  {
    number result(m_measurement_unit);

    if (that == 0)
    {
      throw error(error::type::range, U"Division by zero.");
    }
    mpfr_div_si(result.m_value, m_value, that, default_rounding);

    return result;
  }

  number number::operator/(double that) const
  {
    number result(m_measurement_unit);

    if (that == 0)
    {
      throw error(error::type::range, U"Division by zero.");
    }
    mpfr_div_d(result.m_value, m_value, that, default_rounding);

    return result;
  }

  number& number::operator+=(std::int64_t that)
  {
    mpfr_add_si(m_value, m_value, that, default_rounding);

    return *this;
  }

  number& number::operator++()
  {
    mpfr_add_si(m_value, m_value, 1, default_rounding);

    return *this;
  }

  number& number::operator--()
  {
    mpfr_sub_si(m_value, m_value, 1, default_rounding);

    return *this;
  }

  number number::operator++(int)
  {
    const auto copy(*this);

    mpfr_add_si(m_value, m_value, 1, default_rounding);

    return copy;
  }

  number number::operator--(int)
  {
    const auto copy(*this);

    mpfr_sub_si(m_value, m_value, 1, default_rounding);

    return copy;
  }

  number number::operator-() const
  {
    number result(*this);

    mpfr_neg(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::exp() const
  {
    number result(*this);

    mpfr_exp(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::exp2() const
  {
    number result(*this);

    mpfr_exp2(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::expm1() const
  {
    number result(*this);

    mpfr_expm1(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::log() const
  {
    number result(*this);

    mpfr_log(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::log10() const
  {
    number result(*this);

    mpfr_log10(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::log2() const
  {
    number result(*this);

    mpfr_log2(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::log1p() const
  {
    number result(*this);

    mpfr_log1p(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::pow(const number& exp) const
  {
    number result(*this);

    unit_check(*this, exp);
    mpfr_pow(result.m_value, m_value, exp.m_value, default_rounding);

    return *this;
  }

  number number::sqrt() const
  {
    number result(*this);

    mpfr_sqrt(result.m_value, m_value, default_rounding);

    return *this;
  }

  number number::cbrt() const
  {
    number result(*this);

    mpfr_cbrt(result.m_value, m_value, default_rounding);

    return *this;
  }

  number number::hypot(const number& that) const
  {
    number result(*this);

    mpfr_hypot(result.m_value, m_value, that.m_value, default_rounding);

    return *this;
  }

  number number::cos() const
  {
    number result(*this);

    mpfr_cos(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::sin() const
  {
    number result(*this);

    mpfr_sin(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::tan() const
  {
    number result(*this);

    mpfr_tan(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::acos() const
  {
    number result(*this);

    mpfr_acos(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::asin() const
  {
    number result(*this);

    mpfr_asin(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::atan() const
  {
    number result(*this);

    mpfr_atan(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::atan2(const number& that) const
  {
    number result(*this);

    unit_check(*this, that);
    mpfr_atan2(result.m_value, m_value, that.m_value, default_rounding);

    return result;
  }

  number number::sinh() const
  {
    number result(*this);

    mpfr_sinh(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::cosh() const
  {
    number result(*this);

    mpfr_cosh(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::tanh() const
  {
    number result(*this);

    mpfr_tanh(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::asinh() const
  {
    number result(*this);

    mpfr_asinh(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::acosh() const
  {
    number result(*this);

    mpfr_acosh(result.m_value, m_value, default_rounding);

    return result;
  }

  number number::atanh() const
  {
    number result(*this);

    mpfr_atanh(result.m_value, m_value, default_rounding);

    return result;
  }

  std::ostream& number::output(std::ostream& os) const
  {
    const std::ios::fmtflags flags = os.flags();
    std::ostringstream format;
    char* buffer = nullptr;

    if ((flags & std::ios::showpos))
    {
      format << "%+";
    } else {
      format << "%";
    }
    if (os.precision() > 0)
    {
      format << '.' << os.precision() << "R*";
      if ((flags & std::ios::floatfield) == std::ios::fixed)
      {
        format << 'f';
      }
      else if ((flags & std::ios::floatfield) == std::ios::scientific)
      {
        format << 'e';
      } else {
        format << 'g';
      }
    } else {
      format << "R*e";
    }
    if (mpfr_asprintf(
      &buffer,
      format.str().c_str(),
      default_rounding,
      m_value
    ) >= 0)
    {
      std::string result(buffer);

      mpfr_free_str(buffer);
      if (m_measurement_unit)
      {
        result.append(
          peelo::unicode::encoding::utf8::encode(m_measurement_unit->symbol())
        );
      }
      os << result;
    }

    return os;
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

  static void to_base_unit(mpfr_t result, const class number& number)
  {
    const auto& value = number.value();
    const auto& unit = number.measurement_unit();

    if (unit && !unit->is_base_unit())
    {
      const auto multiplier = unit->multiplier();

      if (multiplier > 0)
      {
        mpfr_init(result);
        mpfr_mul_si(result, value, multiplier, default_rounding);
        return;
      }
      else if (multiplier < 0)
      {
        mpfr_init(result);
        mpfr_div_si(result, value, -multiplier, default_rounding);
        return;
      }
    }

    mpfr_init_set(result, value, default_rounding);
  }

  static void binary_op(
    const number& a,
    const number& b,
    number::value_type result,
    number::unit_type& result_unit,
    binary_op_callback callback
  )
  {
    const auto base = base_unit_of(a);
    number::value_type base_value_a;
    number::value_type base_value_b;

    unit_check(a, b);
    to_base_unit(base_value_a, a);
    to_base_unit(base_value_b, b);
    callback(result, base_value_a, base_value_b, default_rounding);
    mpfr_clear(base_value_a);
    mpfr_clear(base_value_b);

    if (base)
    {
      for (const auto& unit : unit::all_units_of(base->type()))
      {
        const auto multiplier = unit.multiplier();

        if (multiplier > 0 && mpfr_cmp_si(result, multiplier) >= 0)
        {
          mpfr_div_si(result, result, multiplier, default_rounding);
          result_unit = unit;
          return;
        }
      }
    }

    result_unit = base;
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
