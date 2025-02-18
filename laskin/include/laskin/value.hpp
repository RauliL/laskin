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

#include <memory>
#include <vector>

#include <peelo/chrono/date.hpp>
#include <peelo/chrono/time.hpp>
#include <tsl/ordered_map.h>

#include "laskin/number.hpp"

namespace laskin
{
  class node;
  class quote;

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
      weekday
    };

    /**
     * Constructs boolean value.
     */
    static value make_boolean(bool value);

    /**
     * Constructs number value.
     */
    static value make_number(const number& value);

    /**
     * Constructs integer number value.
     */
    static value make_number(
      int value,
      const number::unit_type& unit = std::nullopt
    );

    /**
     * Constructs integer number value.
     */
    static value make_number(
      std::int64_t value,
      const number::unit_type& unit = std::nullopt
    );

    /**
     * Constructs real number value.
     */
    static value make_number(
      double value,
      const number::unit_type& unit = std::nullopt
    );

    /**
     * Constructs number value by parsing number and unit from given string.
     */
    static value make_number(const std::u32string& input);

    /**
     * Constructs vector value.
     */
    static value make_vector(const std::vector<value>& elements);

    /**
     * Constructs vector value from given iterator.
     */
    template<class InputIt>
    static value make_vector(InputIt first, InputIt last)
    {
      return make_vector(std::vector<value>(first, last));
    }

    /**
     * Constructs string value.
     */
    static value make_string(const std::u32string& string);

    /**
     * Constructs string value from given iterator.
     */
    template<class InputIt>
    static value make_string(InputIt first, InputIt last)
    {
      return make_string(std::u32string(first, last));
    }

    /**
     * Constructs quote value.
     */
    static value make_quote(const class quote& quote);

    /**
     * Constructs quote from given sequence of nodes.
     */
    static value make_quote(const std::vector<std::shared_ptr<node>>& nodes);

    /**
     * Constructs month value.
     */
    static value make_month(peelo::chrono::month month);

    /**
     * Constructs month value from given string which should contain valid name
     * of a month.
     */
    static value make_month(const std::u32string& month);

    /**
     * Constructs weekday value.
     */
    static value make_weekday(peelo::chrono::weekday weekday);

    /**
     * Constructs weekday value from given string which should contain valid name
     * of weekday.
     */
    static value make_weekday(const std::u32string& weekday);

    /**
     * Constructs date value.
     */
    static value make_date(const peelo::chrono::date& date);

    /**
     * Constructs date value from given string. The input is expected to be in
     * ISO 8601 format.
     */
    static value make_date(const std::u32string& input);

    /**
     * Constructs time value.
     */
    static value make_time(const peelo::chrono::time& time);

    /**
     * Constructs time value from given string. The input is expected to be in
     * ISO 8601 format.
     */
    static value make_time(const std::u32string& input);

    /**
     * Constructs record value.
     */
    static value make_record(
      const tsl::ordered_map<std::u32string, value>& properties
    );

    /**
     * Constructs boolean value of false.
     */
    explicit value();

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
     * Copies contents of another value into this one.
     */
    value& operator=(const value& that);

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
    const std::vector<value>& as_vector() const;
    const tsl::ordered_map<std::u32string, value>& as_record() const;
    const std::u32string& as_string() const;
    const quote& as_quote() const;
    peelo::chrono::month as_month() const;
    peelo::chrono::weekday as_weekday() const;
    const peelo::chrono::date& as_date() const;
    const peelo::chrono::time& as_time() const;

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

    inline value operator-(const value& that) const
    {
      return substract(that);
    }

    inline value operator*(const value& that) const
    {
      return multiply(that);
    }

    inline value operator/(const value& that) const
    {
      return divide(that);
    }

  private:
    /** Type of the value. */
    enum type m_type;
    union
    {
      bool m_value_boolean;
      number* m_value_number;
      std::vector<value>* m_value_vector;
      std::u32string* m_value_string;
      quote* m_value_quote;
      peelo::chrono::month m_value_month;
      peelo::chrono::weekday m_value_weekday;
      peelo::chrono::date* m_value_date;
      peelo::chrono::time* m_value_time;
      tsl::ordered_map<std::u32string, value>* m_value_record;
    };
  };

  std::ostream& operator<<(std::ostream&, enum value::type);
  std::ostream& operator<<(std::ostream&, const value&);
}
