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
#pragma once

#include <iostream>

#include <mpfr.h>

#include "laskin/unit.hpp"

namespace laskin
{
  /**
   * Combination of number and measurement unit.
   */
  class number
  {
  public:
    using value_type = mpfr_t;
    using unit_type = std::optional<unit>;

    /**
     * Tests whether given string contains valid number.
     */
    static bool is_valid(const std::u32string& input);

    /**
     * Attempts to parse given string into a number.
     */
    static number parse(const std::u32string& input);

    /**
     * Constructs number with value of zero and optional measurement unit.
     */
    number(const unit_type& measurement_unit = std::nullopt);

    /**
     * Constructs number from with given integer value and optional measurement
     * unit.
     */
    number(
      std::int64_t value,
      const unit_type& measurement_unit = std::nullopt
    );

    /**
     * Constructs number from with given real value and optional measurement
     * unit.
     */
    number(
      double value,
      const unit_type& measurement_unit = std::nullopt
    );

    /**
     * Constructs copy of existing number.
     */
    number(const number& that);

    /**
     * Releases all resources allocated by this number instance.
     */
    ~number();

    /**
     * Returns the numeric value of the number.
     */
    inline const value_type& value() const
    {
      return m_value;
    }

    /**
     * Returns the optional measurement unit of the number, or empty value if
     * the number doesn't have one.
     */
    inline const unit_type& measurement_unit() const
    {
      return m_measurement_unit;
    }

    /**
     * Converts number into C++ long integer, or throws an exception if the
     * number is too large for that data type.
     */
    long to_long() const;

    /**
     * Converts number into C++ double precision, or throws an exception if the
     * number is too large for that data type.
     */
    double to_double() const;

    /**
     * Constructs copy of the number without the measurement unit.
     */
    number without_unit() const;

    number& operator=(const number& that);

    int compare(const number& that) const;

    inline bool operator==(const number& that) const
    {
      return compare(that) == 0;
    }

    inline bool operator!=(const number& that) const
    {
      return compare(that) != 0;
    }

    inline bool operator<(const number& that) const
    {
      return compare(that) < 0;
    }

    inline bool operator>(const number& that) const
    {
      return compare(that) > 0;
    }

    inline bool operator<=(const number& that) const
    {
      return compare(that) <= 0;
    }

    inline bool operator>=(const number& that) const
    {
      return compare(that) >= 0;
    }

    number operator+(const number& that) const;
    number operator-(const number& that) const;
    number operator*(const number& that) const;
    number operator*(std::int64_t that) const;
    number operator/(const number& that) const;
    number operator/(std::int64_t that) const;
    number operator/(double that) const;

    number& operator+=(std::int64_t that);

    number& operator++();
    number& operator--();

    number operator++(int);
    number operator--(int);

    number operator-() const;

    inline explicit operator bool() const
    {
      return m_value != 0;
    }

    inline bool operator!() const
    {
      return m_value == 0;
    }

    std::ostream& output(std::ostream& os) const;

  private:
    value_type m_value;
    unit_type m_measurement_unit;
  };

  inline std::ostream& operator<<(
    std::ostream& os,
    const class number& number
  )
  {
    return number.output(os);
  }
}
