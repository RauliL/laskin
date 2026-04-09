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
#include <cstring>
#include <sstream>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/chrono.hpp"
#include "laskin/error.hpp"
#include "laskin/quote.hpp"
#include "laskin/utils.hpp"

namespace laskin
{
  value
  value::parse_number(const std::u32string& input)
  {
    try
    {
      return number::parse(input);
    }
    catch (const std::invalid_argument&)
    {
      throw error(error::type::range, U"Input does not contain valid number.");
    }
  }

  value::value()
    : m_type(type::boolean)
    , m_value_boolean(false) {}

  value::value(bool value)
    : m_type(type::boolean)
    , m_value_boolean(value) {}

  value::value(const number& value)
    : m_type(type::number)
    , m_value_number(new number(value)) {}

  value::value(int value)
    : m_type(type::number)
    , m_value_number(new number(value)) {}

  value::value(long value)
    : m_type(type::number)
    , m_value_number(new number(value)) {}

  value::value(double value)
    : m_type(type::number)
    , m_value_number(new number(value)) {}

  value::value(const std::u32string& value)
    : m_type(type::string)
    , m_value_string(new std::u32string(value)) {}

  value::value(const std::string& value)
    : m_type(type::string)
    , m_value_string(new std::u32string(
        peelo::unicode::encoding::utf8::decode(value))
      ) {}

  value::value(const vector& elements)
    : m_type(type::vector)
    , m_value_vector(new vector(elements)) {}

  value::value(const record& properties)
    : m_type(type::record)
    , m_value_record(new record(properties)) {}

  value::value(const quote& value)
    : m_type(type::quote)
    , m_value_quote(new quote(value)) {}

  value::value(const date& value)
    : m_type(type::date)
    , m_value_date(new date(value)) {}

  value::value(const time& value)
    : m_type(type::time)
    , m_value_time(new time(value)) {}

  value::value(month value)
    : m_type(type::month)
    , m_value_month(value) {}

  value::value(weekday value)
    : m_type(type::weekday)
    , m_value_weekday(value) {}

  value::value(const value& that)
    : m_type(that.m_type)
  {
    switch (m_type)
    {
      case type::boolean:
        m_value_boolean = that.m_value_boolean;
        break;

      case type::number:
        m_value_number = new number(*that.m_value_number);
        break;

      case type::vector:
        m_value_vector = new vector(*that.m_value_vector);
        break;

      case type::string:
        m_value_string = new std::u32string(*that.m_value_string);
        break;

      case type::quote:
        m_value_quote = new quote(*that.m_value_quote);
        break;

      case type::month:
        m_value_month = that.m_value_month;
        break;

      case type::weekday:
        m_value_weekday = that.m_value_weekday;
        break;

      case type::date:
        m_value_date = new date(*that.m_value_date);
        break;

      case type::time:
        m_value_time = new time(*that.m_value_time);
        break;

      case type::record:
        m_value_record = new record(*that.m_value_record);
        break;
    }
  }

  value::value(value&& that)
    : m_type(that.m_type)
  {
    switch (m_type)
    {
      case type::boolean:
        m_value_boolean = that.m_value_boolean;
        break;

      case type::number:
        m_value_number = that.m_value_number;
        break;

      case type::vector:
        m_value_vector = that.m_value_vector;
        break;

      case type::string:
        m_value_string = that.m_value_string;
        break;

      case type::quote:
        m_value_quote = that.m_value_quote;
        break;

      case type::month:
        m_value_month = that.m_value_month;
        break;

      case type::weekday:
        m_value_weekday = that.m_value_weekday;
        break;

      case type::date:
        m_value_date = that.m_value_date;
        break;

      case type::time:
        m_value_time = that.m_value_time;
        break;

      case type::record:
        m_value_record = that.m_value_record;
        break;
    }
    that.m_type = type::boolean;
    that.m_value_boolean = false;
  }

  value::~value()
  {
    reset();
  }

  value&
  value::assign(bool value)
  {
    reset();
    m_type = type::boolean;
    m_value_boolean = value;

    return *this;
  }

  value&
  value::assign(const number& value)
  {
    reset();
    m_type = type::number;
    m_value_number = new number(value);

    return *this;
  }

  value&
  value::assign(int value)
  {
    reset();
    m_type = type::number;
    m_value_number = new number(value);

    return *this;
  }

  value&
  value::assign(long value)
  {
    reset();
    m_type = type::number;
    m_value_number = new number(value);

    return *this;
  }

  value&
  value::assign(double value)
  {
    reset();
    m_type = type::number;
    m_value_number = new number(value);

    return *this;
  }

  value&
  value::assign(const std::u32string& value)
  {
    reset();
    m_type = type::string;
    m_value_string = new std::u32string(value);

    return *this;
  }

  value&
  value::assign(const std::string& value)
  {
    using peelo::unicode::encoding::utf8::decode;

    reset();
    m_type = type::string;
    m_value_string = new std::u32string(decode(value));

    return *this;
  }

  value&
  value::assign(const vector& elements)
  {
    reset();
    m_type = type::vector;
    m_value_vector = new vector(elements);

    return *this;
  }

  value&
  value::assign(const record& properties)
  {
    reset();
    m_type = type::record;
    m_value_record = new record(properties);

    return *this;
  }

  value&
  value::assign(const quote& value)
  {
    reset();
    m_type = type::quote;
    m_value_quote = new quote(value);

    return *this;
  }

  value&
  value::assign(const date& value)
  {
    reset();
    m_type = type::date;
    m_value_date = new date(value);

    return *this;
  }

  value&
  value::assign(const time& value)
  {
    reset();
    m_type = type::time;
    m_value_time = new time(value);

    return *this;
  }

  value&
  value::assign(month value)
  {
    reset();
    m_type = type::month;
    m_value_month = value;

    return *this;
  }

  value&
  value::assign(weekday value)
  {
    reset();
    m_type = type::weekday;
    m_value_weekday = value;

    return *this;
  }

  value&
  value::assign(const value& that)
  {
    if (this != &that)
    {
      reset();
      switch (m_type = that.m_type)
      {
        case type::boolean:
          m_value_boolean = that.m_value_boolean;
          break;

        case type::number:
          m_value_number = new number(*that.m_value_number);
          break;

        case type::vector:
          m_value_vector = new vector(*that.m_value_vector);
          break;

        case type::string:
          m_value_string = new std::u32string(*that.m_value_string);
          break;

        case type::quote:
          m_value_quote = new quote(*that.m_value_quote);
          break;

        case type::month:
          m_value_month = that.m_value_month;
          break;

        case type::weekday:
          m_value_weekday = that.m_value_weekday;
          break;

        case type::date:
          m_value_date = new date(*that.m_value_date);
          break;

        case type::time:
          m_value_time = new time(*that.m_value_time);
          break;

        case type::record:
          m_value_record = new record(*that.m_value_record);
          break;
      }
    }

    return *this;
  }

  value&
  value::operator=(value&& that)
  {
    if (this != &that)
    {
      reset();
      switch (m_type = that.m_type)
      {
        case type::boolean:
          m_value_boolean = that.m_value_boolean;
          break;

        case type::number:
          m_value_number = that.m_value_number;
          break;

        case type::vector:
          m_value_vector = that.m_value_vector;
          break;

        case type::string:
          m_value_string = that.m_value_string;
          break;

        case type::quote:
          m_value_quote = that.m_value_quote;
          break;

        case type::month:
          m_value_month = that.m_value_month;
          break;

        case type::weekday:
          m_value_weekday = that.m_value_weekday;
          break;

        case type::date:
          m_value_date = that.m_value_date;
          break;

        case type::time:
          m_value_time = that.m_value_time;
          break;

        case type::record:
          m_value_record = that.m_value_record;
          break;
      }
      that.m_type = type::boolean;
      that.m_value_boolean = false;
    }

    return *this;
  }

  std::u32string
  value::type_description(enum type type)
  {
    switch (type)
    {
      case type::boolean:
        return U"boolean";

      case type::number:
        return U"number";

      case type::vector:
        return U"vector";

      case type::string:
        return U"string";

      case type::quote:
        return U"quote";

      case type::month:
        return U"month";

      case type::weekday:
        return U"weekday";

      case type::date:
        return U"date";

      case type::time:
        return U"time";

      case type::record:
        return U"record";
    }

    return U"unknown";
  }

  void
  value::reset()
  {
    switch (m_type)
    {
      case type::number:
        delete m_value_number;
        break;

      case type::vector:
        delete m_value_vector;
        break;

      case type::string:
        delete m_value_string;
        break;

      case type::quote:
        delete m_value_quote;
        break;

      case type::date:
        delete m_value_date;
        break;

      case type::time:
        delete m_value_time;
        break;

      case type::record:
        delete m_value_record;
        break;

      default:
        break;
    }

    m_type = type::boolean;
    m_value_boolean = false;
  }

  bool
  value::as_boolean() const
  {
    if (!is(type::boolean))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting boolean."
      );
    }

    return m_value_boolean;
  }

  const number&
  value::as_number() const
  {
    if (!is(type::number))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting number."
      );
    }

    return *m_value_number;
  }

  const vector&
  value::as_vector() const
  {
    if (!is(type::vector))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting vector."
      );
    }

    return *m_value_vector;
  }

  const record&
  value::as_record() const
  {
    if (!is(type::record))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting record."
      );
    }

    return *m_value_record;
  }

  const std::u32string&
  value::as_string() const
  {
    if (!is(type::string))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was expecting string."
      );
    }

    return *m_value_string;
  }

  const quote&
  value::as_quote() const
  {
    if (!is(type::quote))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting quote."
      );
    }

    return *m_value_quote;
  }

  month
  value::as_month() const
  {
    if (!is(type::month))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting month."
      );
    }

    return m_value_month;
  }

  weekday
  value::as_weekday() const
  {
    if (!is(type::weekday))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting day of week."
      );
    }

    return m_value_weekday;
  }

  const date&
  value::as_date() const
  {
    if (!is(type::date))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting date."
      );
    }

    return *m_value_date;
  }

  const time&
  value::as_time() const
  {
    if (!is(type::time))
    {
      throw error(
        error::type::type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was expecting time."
      );
    }

    return *m_value_time;
  }

  value::operator long() const
  {
    try
    {
      return long(as_number());
    }
    catch (const std::underflow_error&)
    {
      throw error(error::type::range, U"Numeric underflow.");
    }
    catch (const std::overflow_error&)
    {
      throw error(error::type::range, U"Numeric overflow.");
    }
  }

  value::operator double() const
  {
    try
    {
      return double(as_number());
    }
    catch (const std::underflow_error&)
    {
      throw error(error::type::range, U"Numeric underflow.");
    }
    catch (const std::overflow_error&)
    {
      throw error(error::type::range, U"Numeric overflow.");
    }
  }

  static std::u32string
  vector_to_string(const vector& elements)
  {
    std::u32string result;
    bool first = true;

    for (const auto& element : elements)
    {
      if (first)
      {
        first = false;
      } else {
        result.append(U", ");
      }
      result.append(element.to_string());
    }

    return result;
  }

  static std::u32string
  month_to_string(month value)
  {
    switch (value)
    {
      case month::jan:
        return U"january";

      case month::feb:
        return U"february";

      case month::mar:
        return U"march";

      case month::apr:
        return U"april";

      case month::may:
        return U"may";

      case month::jun:
        return U"june";

      case month::jul:
        return U"july";

      case month::aug:
        return U"august";

      case month::sep:
        return U"september";

      case month::oct:
        return U"october";

      case month::nov:
        return U"november";

      case month::dec:
        return U"december";
    }

    return U"unknown";
  }

  static std::u32string
  weekday_to_string(weekday value)
  {
    switch (value)
    {
      case weekday::sun:
        return U"sunday";

      case weekday::mon:
        return U"monday";

      case weekday::tue:
        return U"tuesday";

      case weekday::wed:
        return U"wednesday";

      case weekday::thu:
        return U"thursday";

      case weekday::fri:
        return U"friday";

      case weekday::sat:
        return U"saturday";
    }

    return U"unknown";
  }

  static std::u32string
  date_to_string(const date& value)
  {
    const auto year = value.year();
    const auto month = value.month();
    const auto day = value.day();
    char buffer[32];
    std::u32string result;

    std::snprintf(
      buffer,
      32,
      "%d-%02d-%02d",
      year,
      static_cast<int>(month) + 1,
      day
    );
    result.reserve(std::strlen(buffer));
    for (auto p = buffer; *p; ++p)
    {
      result.append(1, static_cast<char32_t>(*p));
    }

    return result;
  }

  static std::u32string
  time_to_string(const time& value)
  {
    const auto hour = value.hour();
    const auto minute = value.minute();
    const auto second = value.second();
    char buffer[9];
    std::u32string result;

    std::snprintf(
      buffer,
      9,
      "%02d:%02d:%02d",
      hour,
      minute,
      second
    );
    result.reserve(8);
    for (auto p = buffer; *p; ++p)
    {
      result.append(1, static_cast<char32_t>(*p));
    }

    return result;
  }

  static std::u32string
  record_to_string(const record& properties)
  {
    std::u32string result;
    bool first = true;

    for (const auto& property : properties)
    {
      if (first)
      {
        first = false;
      } else {
        result.append(U", ");
      }
      result.append(property.first);
      result.append(U"=");
      result.append(property.second.to_string());
    }

    return result;
  }

  std::u32string
  value::to_string() const
  {
    switch (m_type)
    {
      case type::boolean:
        return m_value_boolean ? U"true" : U"false";

      case type::number:
        return m_value_number->to_u32string();

      case type::vector:
        return vector_to_string(*m_value_vector);

      case type::string:
        return *m_value_string;

      case type::quote:
        return m_value_quote->to_source();

      case type::month:
        return month_to_string(m_value_month);

      case type::weekday:
        return weekday_to_string(m_value_weekday);

      case type::date:
        return date_to_string(*m_value_date);

      case type::time:
        return time_to_string(*m_value_time);

      case type::record:
        return record_to_string(*m_value_record);
    }

    return U"";
  }

  static std::u32string
  vector_to_source(const vector& elements)
  {
    std::u32string result;
    bool first = true;

    result.append(1, U'[');
    for (const auto& element : elements)
    {
      if (first)
      {
        first = false;
      } else {
        result.append(U", ");
      }
      result.append(element.to_source());
    }
    result.append(1, U']');

    return result;
  }

  static std::u32string
  record_to_source(const record& properties)
  {
    std::u32string result;
    bool first = true;

    result.append(1, U'{');
    for (const auto& property : properties)
    {
      if (first)
      {
        first = false;
      } else {
        result.append(U", ");
      }
      result.append(utils::escape_string(property.first));
      result.append(U": ");
      result.append(property.second.to_source());
    }
    result.append(1, U'}');

    return result;
  }

  std::u32string
  value::to_source() const
  {
    switch (m_type)
    {
      case type::boolean:
        return m_value_boolean ? U"true" : U"false";

      case type::number:
        return m_value_number->to_u32string();

      case type::vector:
        return vector_to_source(*m_value_vector);

      case type::string:
        return utils::escape_string(*m_value_string);

      case type::quote:
        return m_value_quote->to_source();

      case type::month:
        return month_to_string(m_value_month);

      case type::weekday:
        return weekday_to_string(m_value_weekday);

      case type::date:
        return date_to_string(*m_value_date);

      case type::time:
        return time_to_string(*m_value_time);

      case type::record:
        return record_to_source(*m_value_record);
    }

    return U"";
  }

  std::ostream&
  operator<<(std::ostream& out, enum value::type type)
  {
    using peelo::unicode::encoding::utf8::encode;

    out << encode(value::type_description(type));

    return out;
  }

  std::ostream&
  operator<<(std::ostream& out, const class value& value)
  {
    using peelo::unicode::encoding::utf8::encode;

    out << encode(value.to_string());

    return out;
  }
}
