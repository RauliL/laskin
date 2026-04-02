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

  using binary_op_callback = int(*)(
    mpfr_t,
    mpfr_srcptr,
    mpfr_srcptr,
    mpfr_rnd_t
  );
  using unary_op_callback = int(*)(
    mpfr_t,
    mpfr_srcptr,
    mpfr_rnd_t
  );

  static void to_base_unit(
    number::value_type,
    const number::value_type,
    const number::unit_type&
  );
  static void binary_op(
    const number::value_type,
    const number::unit_type&,
    const number::value_type,
    const number::unit_type&,
    number::value_type,
    number::unit_type&,
    binary_op_callback
  );
  static void unary_op(
    number::value_type,
    number::unit_type&,
    unary_op_callback
  );
  static void unit_check(
    const number::unit_type&,
    const number::unit_type&
  );

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

  static void
  size_check()
  {
    if (mpfr_underflow_p())
    {
      mpfr_clear_underflow();

      throw error(error::type::range, U"Numeric value is too small.");
    }
    if (mpfr_overflow_p())
    {
      mpfr_clear_overflow();

      throw error(error::type::range, U"Numeric value is too large.");
    }
  }

  number::operator long() const
  {
    const auto result = mpfr_get_si(m_value, default_rounding);

    size_check();

    return result;
  }

  number::operator double() const
  {
    const auto result = mpfr_get_d(m_value, default_rounding);

    size_check();

    return result;
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
    int result;

    unit_check(this->m_measurement_unit, that.m_measurement_unit);
    if (m_measurement_unit)
    {
      number::value_type a;
      number::value_type b;

      to_base_unit(a, m_value, m_measurement_unit);
      to_base_unit(b, that.m_value, that.m_measurement_unit);
      result = mpfr_cmp(a, b);
      mpfr_clear(a);
      mpfr_clear(b);
    } else {
      result = mpfr_cmp(m_value, that.m_value);
    }

    return result;
  }

  number number::operator+(const number& that) const
  {
    number result;

    binary_op(
      m_value,
      m_measurement_unit,
      that.m_value,
      that.m_measurement_unit,
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
      m_value,
      m_measurement_unit,
      that.m_value,
      that.m_measurement_unit,
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
      m_value,
      m_measurement_unit,
      that.m_value,
      that.m_measurement_unit,
      result.m_value,
      result.m_measurement_unit,
      mpfr_mul
    );

    return result;
  }

  number number::operator*(double that) const
  {
    number result(m_measurement_unit);

    mpfr_mul_d(result.m_value, m_value, that, default_rounding);

    return result;
  }

  number number::operator/(const number& that) const
  {
    number result;

    binary_op(
      m_value,
      m_measurement_unit,
      that.m_value,
      that.m_measurement_unit,
      result.m_value,
      result.m_measurement_unit,
      mpfr_div
    );

    return result;
  }

  number number::operator/(double that) const
  {
    number result(m_measurement_unit);

    mpfr_div_d(result.m_value, m_value, that, default_rounding);

    return result;
  }

  number& number::operator+=(double that)
  {
    mpfr_add_d(m_value, m_value, that, default_rounding);

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

    unary_op(result.m_value, result.m_measurement_unit, mpfr_neg);

    return result;
  }

  number number::exp() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_exp);

    return result;
  }

  number number::exp2() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_exp2);

    return result;
  }

  number number::expm1() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_expm1);

    return result;
  }

  number number::log() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_log);

    return result;
  }

  number number::log10() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_log10);

    return result;
  }

  number number::log2() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_log2);

    return result;
  }

  number number::log1p() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_log1p);

    return result;
  }

  number number::pow(const number& exp) const
  {
    number result;

    binary_op(
      m_value,
      m_measurement_unit,
      exp.m_value,
      exp.m_measurement_unit,
      result.m_value,
      result.m_measurement_unit,
      mpfr_pow
    );

    return *this;
  }

  number number::sqrt() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_sqrt);

    return *this;
  }

  number number::cbrt() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_cbrt);

    return *this;
  }

  number number::hypot(const number& that) const
  {
    number result;

    binary_op(
      m_value,
      m_measurement_unit,
      that.m_value,
      that.m_measurement_unit,
      result.m_value,
      result.m_measurement_unit,
      mpfr_hypot
    );

    return *this;
  }

  number number::cos() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_cos);

    return result;
  }

  number number::sin() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_sin);

    return result;
  }

  number number::tan() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_tan);

    return result;
  }

  number number::acos() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_acos);

    return result;
  }

  number number::asin() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_asin);

    return result;
  }

  number number::atan() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_atan);

    return result;
  }

  number number::atan2(const number& that) const
  {
    number result;

    binary_op(
      m_value,
      m_measurement_unit,
      that.m_value,
      that.m_measurement_unit,
      result.m_value,
      result.m_measurement_unit,
      mpfr_atan2
    );

    return result;
  }

  number number::sinh() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_sinh);

    return result;
  }

  number number::cosh() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_cosh);

    return result;
  }

  number number::tanh() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_tanh);

    return result;
  }

  number number::asinh() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_asinh);

    return result;
  }

  number number::acosh() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_acosh);

    return result;
  }

  number number::atanh() const
  {
    number result(*this);

    unary_op(result.m_value, result.m_measurement_unit, mpfr_atanh);

    return result;
  }

  std::string number::to_string() const
  {
    using peelo::unicode::encoding::utf8::encode;

    std::string result;
    char* buffer = nullptr;
    const auto length = mpfr_asprintf(
      &buffer,
      "%.10R*g",
      default_rounding,
      m_value
    );

    if (length >= 0)
    {
      result.append(buffer, length);
      mpfr_free_str(buffer);
      if (m_measurement_unit)
      {
        result.append(encode(m_measurement_unit->symbol()));
      }
    }

    return result;
  }

  static void
  to_base_unit(
    number::value_type result,
    const number::value_type value,
    const number::unit_type& unit
  )
  {
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

  static void
  normalize_unit(
    number::value_type result_value,
    number::unit_type& result_unit
  )
  {
    const auto base = unit::base_unit_of(result_unit->type());

    for (const auto& unit : unit::all_units_of(base.type()))
    {
      const auto multiplier = unit.multiplier();

      if (multiplier > 0 && mpfr_cmp_si(result_value, multiplier) >= 0)
      {
        mpfr_div_si(result_value, result_value, multiplier, default_rounding);
        result_unit = unit;
        return;
      }
    }
    result_unit = base;
  }

  static void
  binary_op(
    const number::value_type a_value,
    const number::unit_type& a_unit,
    const number::value_type b_value,
    const number::unit_type& b_unit,
    number::value_type result,
    number::unit_type& result_unit,
    binary_op_callback callback
  )
  {
    unit_check(a_unit, b_unit);
    if (a_unit)
    {
      const auto base = unit::base_unit_of(a_unit->type());
      number::value_type base_value_a;
      number::value_type base_value_b;

      to_base_unit(base_value_a, a_value, a_unit);
      to_base_unit(base_value_b, b_value, b_unit);
      callback(result, base_value_a, base_value_b, default_rounding);
      mpfr_clear(base_value_a);
      mpfr_clear(base_value_b);
      normalize_unit(result, result_unit);
    } else {
      callback(result, a_value, b_value, default_rounding);
    }
  }

  static void
  unary_op(
    number::value_type result,
    number::unit_type& result_unit,
    unary_op_callback callback
  )
  {
    callback(result, result, default_rounding);
    if (result_unit)
    {
      normalize_unit(result, result_unit);
    }
  }

  static void
  unit_check(
    const number::unit_type& a,
    const number::unit_type& b
  )
  {
    if (a && b)
    {
      const auto type_a = a->type();
      const auto type_b = b->type();

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
    else if (b)
    {
      throw error(
        error::type::unit,
        U"Cannot compare number without an unit against number which has " +
        to_string(b->type()) +
        U" as measurement unit."
      );
    }
  }
}
