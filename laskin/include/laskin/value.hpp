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
#pragma once

#include <memory>

#include "laskin/types.hpp"

namespace laskin
{
  /**
   * Wrapper class for supported value types.
   */
  class value
  {
  public:
    /**
     * Enumeration of different supported value types.
     */
    enum class type
    {
      boolean,
      date,
      month,
      number,
      quote,
      record,
      string,
      time,
      vector,
      weekday,
    };

    /**
     * Constructs number value by parsing number and unit from given string.
     */
    static value parse_number(const std::u32string& input);

    /**
     * Constructs boolean value of false.
     */
    explicit value();

    /**
     * Constructs boolean value.
     */
    value(bool value);

    /**
     * Constructs numeric value.
     */
    value(const number& value);

    /**
     * Constructs numeric value.
     */
    value(int value);

    /**
     * Constructs numeric value.
     */
    value(long value);

    /**
     * Constructs numeric value.
     */
    value(double value);

    /**
     * Constructs string.
     */
    value(const std::u32string& value);

    /**
     * Constructs string. The input is expected to be encoded with UTF-8.
     */
    value(const std::string& value);

    /**
     * Constructs vector.
     */
    value(const vector& elements);

    /**
     * Constructs record.
     */
    value(const record& properties);

    /**
     * Constructs quote.
     */
    value(const quote& value);

    /**
     * Constructs date value.
     */
    value(const date& value);

    /**
     * Constructs time value.
     */
    value(const time& value);

    /**
     * Constructs month value.
     */
    value(month value);

    /**
     * Constructs day of the week value.
     */
    value(weekday value);

    /**
     * Constructs copy of existing value.
     */
    value(const value& that);

    /**
     * Move constructor which transfers contents from another value into this
     * one.
     */
    value(value&& that);

    /**
     * Destructor.
     */
    ~value();

    /**
     * Assigns boolean value into this value.
     */
    value& assign(bool value);

    /**
     * Assigns numeric value into this value.
     */
    value& assign(const number& value);

    /**
     * Assigns numeric value into this value.
     */
    value& assign(int value);

    /**
     * Assigns numeric value into this value.
     */
    value& assign(long value);

    /**
     * Assigns numeric value into this value.
     */
    value& assign(double value);

    /**
     * Assigns string into this value.
     */
    value& assign(const std::u32string& value);

    /**
     * Assigns UTF-8 encoded into this value.
     */
    value& assign(const std::string& value);

    /**
     * Assigns vector into this value.
     */
    value& assign(const vector& elements);

    /**
     * Assigns record into this value.
     */
    value& assign(const record& properties);

    /**
     * Assigns quote into this value.
     */
    value& assign(const quote& value);

    /**
     * Assigns date value into this value.
     */
    value& assign(const date& value);

    /**
     * Assigns time value into this value.
     */
    value& assign(const time& value);

    /**
     * Assigns month value into this value.
     */
    value& assign(month value);

    /**
     * Assigns day of the week value into this value.
     */
    value& assign(weekday value);

    /**
     * Copies contents of another value into this one.
     */
    value& assign(const value& that);

    /**
     * Assigns boolean value into this value.
     */
    inline value& operator=(bool value)
    {
      return assign(value);
    }

    /**
     * Assigns numeric value into this value.
     */
    inline value& operator=(const number& value)
    {
      return assign(value);
    }

    /**
     * Assigns numeric value into this value.
     */
    inline value& operator=(int value)
    {
      return assign(value);
    }

    /**
     * Assigns numeric value into this value.
     */
    inline value& operator=(long value)
    {
      return assign(value);
    }

    /**
     * Assigns numeric value into this value.
     */
    inline value& operator=(double value)
    {
      return assign(value);
    }

    /**
     * Assigns string into this value.
     */
    inline value& operator=(const std::u32string& value)
    {
      return assign(value);
    }

    /**
     * Assigns UTF-8 encoded into this value.
     */
    inline value& operator=(const std::string& value)
    {
      return assign(value);
    }

    /**
     * Assigns vector into this value.
     */
    inline value& operator=(const vector& elements)
    {
      return assign(elements);
    }

    /**
     * Assigns record into this value.
     */
    inline value& operator=(const record& properties)
    {
      return assign(properties);
    }

    /**
     * Assigns quote into this value.
     */
    inline value& operator=(const quote& value)
    {
      return assign(value);
    }

    /**
     * Assigns date value into this value.
     */
    inline value& operator=(const date& value)
    {
      return assign(value);
    }

    /**
     * Assigns time value into this value.
     */
    inline value& operator=(const time& value)
    {
      return assign(value);
    }

    /**
     * Assigns day of the week value into this value.
     */
    inline value& operator=(weekday value)
    {
      return assign(value);
    }

    /**
     * Copies contents of another value into this one.
     */
    inline value& operator=(const value& that)
    {
      return assign(that);
    }

    /**
     * Moves contents of another value into this one.
     */
    value& operator=(value&& that);

    /**
     * Returns type of the value.
     */
    inline enum type type() const
    {
      return m_type;
    }

    /**
     * Tests whether the value is of given type.
     */
    inline bool is(enum type type) const
    {
      return m_type == type;
    }

    /**
     * Returns textual description of a value type.
     */
    static std::u32string type_description(enum type type);

    /**
     * Resets the value back to it's initial state.
     */
    void reset();

    bool as_boolean() const;
    const number& as_number() const;
    const vector& as_vector() const;
    const record& as_record() const;
    const std::u32string& as_string() const;
    const quote& as_quote() const;
    month as_month() const;
    weekday as_weekday() const;
    const date& as_date() const;
    const time& as_time() const;

    /**
     * Extracts number value as long integer, or throws `laskin::error` if
     * the value does not contain number value or does not fit into long
     * integer.
     */
    explicit operator long() const;

    /**
     * Extracts number value as double precision, or throws `laskin::error` if
     * the value does not contain number value or does not fit into double
     * precision.
     */
    explicit operator double() const;

    /**
     * Constructs string representation of the value.
     */
    std::u32string to_string() const;

    /**
     * Constructs the most appropriate representation of what the value would
     * look like in source code.
     */
    std::u32string to_source() const;

    /**
     * Tests whether the value is equal with the one given as argument.
     */
    bool equals(const value& that) const;

    /**
     * Compares two values against each other.
     */
    int compare(const value& that) const;

    /**
     * Adds another value into this one.
     */
    value add(const value& that) const;

    /**
     * Substracts given value from this one.
     */
    value substract(const value& that) const;

    /**
     * Multiplies this value with the given value.
     */
    value multiply(const value& that) const;

    /**
     * Divides this value with the given value.
     */
    value divide(const value& that) const;

    /**
     * Performs modulo operation.
     */
    value modulo(const value& that) const;

    inline bool operator==(const value& that) const
    {
      return equals(that);
    }

    inline bool operator!=(const value& that) const
    {
      return !equals(that);
    }

    inline bool operator<(const value& that) const
    {
      return compare(that) < 0;
    }

    inline bool operator>(const value& that) const
    {
      return compare(that) > 0;
    }

    inline bool operator<=(const value& that) const
    {
      return compare(that) <= 0;
    }

    inline bool operator>=(const value& that) const
    {
      return compare(that) >= 0;
    }

    inline value operator+(const value& that) const
    {
      return add(that);
    }

    inline value& operator+=(const value& that)
    {
      return assign(add(that));
    }

    inline value operator-(const value& that) const
    {
      return substract(that);
    }

    inline value& operator-=(const value& that)
    {
      return assign(substract(that));
    }

    inline value operator*(const value& that) const
    {
      return multiply(that);
    }

    inline value& operator*=(const value& that)
    {
      return assign(multiply(that));
    }

    inline value operator/(const value& that) const
    {
      return divide(that);
    }

    inline value& operator/=(const value& that)
    {
      return assign(divide(that));
    }

    inline value operator%(const value& that) const
    {
      return modulo(that);
    }

    inline value& operator%=(const value& that)
    {
      return assign(modulo(that));
    }

  private:
    /** Type of the value. */
    enum type m_type;
    union
    {
      bool m_value_boolean;
      number* m_value_number;
      vector* m_value_vector;
      std::u32string* m_value_string;
      quote* m_value_quote;
      month m_value_month;
      weekday m_value_weekday;
      date* m_value_date;
      time* m_value_time;
      record* m_value_record;
    };
  };

  std::ostream& operator<<(std::ostream&, enum value::type);
  std::ostream& operator<<(std::ostream&, const value&);
}
