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
#include <cstring>
#include <sstream>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/chrono.hpp"
#include "laskin/error.hpp"
#include "laskin/quote.hpp"
#include "laskin/utils.hpp"

namespace laskin
{
  value value::make_boolean(bool value)
  {
    class value instance;

    instance.m_value_boolean = value;

    return instance;
  }

  value value::make_number(const number& value)
  {
    class value instance;

    instance.m_type = type::number;
    instance.m_value_number = new number(value);

    return instance;
  }

  value value::make_number(int value, const number::unit_type& unit)
  {
    return make_number(static_cast<std::int64_t>(value), unit);
  }

  value value::make_number(std::int64_t value, const number::unit_type& unit)
  {
    class value instance;

    instance.m_type = type::number;
    instance.m_value_number = new number(value, unit);

    return instance;
  }

  value value::make_number(double value, const number::unit_type& unit)
  {
    class value instance;

    instance.m_type = type::number;
    instance.m_value_number = new number(value, unit);

    return instance;
  }

  value value::make_number(const std::u32string& input)
  {
    class value instance;

    instance.m_type = type::number;
    instance.m_value_number = new number(number::parse(input));

    return instance;
  }

  value value::make_vector(const std::vector<value>& elements)
  {
    class value instance;

    instance.m_type = type::vector;
    instance.m_value_vector = new std::vector<value>(elements);

    return instance;
  }

  value value::make_record(
    const std::unordered_map<std::u32string, value>& properties
  )
  {
    class value instance;

    instance.m_type = type::record;
    instance.m_value_record = new std::unordered_map<
      std::u32string,
      value
    >(properties);

    return instance;
  }

  value value::make_string(const std::u32string& string)
  {
    value instance;

    instance.m_type = type::string;
    instance.m_value_string = new std::u32string(string);

    return instance;
  }

  value value::make_quote(const class quote& quote)
  {
    class value instance;

    instance.m_type = type::quote;
    instance.m_value_quote = new class quote(quote);

    return instance;
  }

  value value::make_quote(const quote::node_container& nodes)
  {
    class value instance;

    instance.m_type = type::quote;
    instance.m_value_quote = new class quote(nodes);

    return instance;
  }

  value value::make_month(peelo::chrono::month month)
  {
    value instance;

    instance.m_type = type::month;
    instance.m_value_month = month;

    return instance;
  }

  value value::make_weekday(peelo::chrono::weekday weekday)
  {
    value instance;

    instance.m_type = type::weekday;
    instance.m_value_weekday = weekday;

    return instance;
  }

  value value::make_date(const peelo::chrono::date& date)
  {
    value instance;

    instance.m_type = type::date;
    instance.m_value_date = new peelo::chrono::date(date);

    return instance;
  }

  value value::make_date(const std::u32string& input)
  {
    const auto date = parse_date(input);
    value instance;

    instance.m_type = type::date;
    instance.m_value_date = new peelo::chrono::date(date);

    return instance;
  }

  value value::make_time(const peelo::chrono::time& time)
  {
    value instance;

    instance.m_type = type::time;
    instance.m_value_time = new peelo::chrono::time(time);

    return instance;
  }

  value value::make_time(const std::u32string& input)
  {
    const auto time = parse_time(input);
    value instance;

    instance.m_type = type::time;
    instance.m_value_time = new peelo::chrono::time(time);

    return instance;
  }

  value::value()
    : m_type(type::boolean)
    , m_value_boolean(false) {}

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
        m_value_vector = new std::vector<value>(*that.m_value_vector);
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
        m_value_date = new peelo::chrono::date(*that.m_value_date);
        break;

      case type::time:
        m_value_time = new peelo::chrono::time(*that.m_value_time);
        break;

      case type::record:
        m_value_record = new std::unordered_map<std::u32string, value>(
          *that.m_value_record
        );
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

  value& value::operator=(const value& that)
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
          m_value_vector = new std::vector<value>(*that.m_value_vector);
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
          m_value_date = new peelo::chrono::date(*that.m_value_date);
          break;

        case type::time:
          m_value_time = new peelo::chrono::time(*that.m_value_time);
          break;

        case type::record:
          m_value_record = new std::unordered_map<std::u32string, value>(
            *that.m_value_record
          );
          break;
      }
    }

    return *this;
  }

  value& value::operator=(value&& that)
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

  std::u32string value::type_description(enum type type)
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

  void value::reset()
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

  bool value::as_boolean() const
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

  const number& value::as_number() const
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

  const std::vector<value>& value::as_vector() const
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

  const std::unordered_map<std::u32string, value>& value::as_record() const
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

  const std::u32string& value::as_string() const
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

  const quote& value::as_quote() const
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

  peelo::chrono::month value::as_month() const
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

  peelo::chrono::weekday value::as_weekday() const
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

  const peelo::chrono::date& value::as_date() const
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

  const peelo::chrono::time& value::as_time() const
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

  static std::u32string number_to_string(const number& value)
  {
    std::stringstream ss;

    ss << value;

    return peelo::unicode::encoding::utf8::decode(ss.str());
  }

  static std::u32string vector_to_string(const std::vector<value>& elements)
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

  static std::u32string month_to_string(peelo::chrono::month month)
  {
    switch (month)
    {
      case peelo::chrono::month::jan:
        return U"january";

      case peelo::chrono::month::feb:
        return U"february";

      case peelo::chrono::month::mar:
        return U"march";

      case peelo::chrono::month::apr:
        return U"april";

      case peelo::chrono::month::may:
        return U"may";

      case peelo::chrono::month::jun:
        return U"june";

      case peelo::chrono::month::jul:
        return U"july";

      case peelo::chrono::month::aug:
        return U"august";

      case peelo::chrono::month::sep:
        return U"september";

      case peelo::chrono::month::oct:
        return U"october";

      case peelo::chrono::month::nov:
        return U"november";

      case peelo::chrono::month::dec:
        return U"december";
    }

    return U"unknown";
  }

  static std::u32string weekday_to_string(peelo::chrono::weekday weekday)
  {
    switch (weekday)
    {
      case peelo::chrono::weekday::sun:
        return U"sunday";

      case peelo::chrono::weekday::mon:
        return U"monday";

      case peelo::chrono::weekday::tue:
        return U"tuesday";

      case peelo::chrono::weekday::wed:
        return U"wednesday";

      case peelo::chrono::weekday::thu:
        return U"thursday";

      case peelo::chrono::weekday::fri:
        return U"friday";

      case peelo::chrono::weekday::sat:
        return U"saturday";
    }

    return U"unknown";
  }

  static std::u32string date_to_string(const peelo::chrono::date& date)
  {
    const auto year = date.year();
    const auto month = date.month();
    const auto day = date.day();
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

  static std::u32string time_to_string(const peelo::chrono::time& time)
  {
    const auto hour = time.hour();
    const auto minute = time.minute();
    const auto second = time.second();
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

  static std::u32string record_to_string(
    const std::unordered_map<std::u32string, value>& properties
  )
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

  std::u32string value::to_string() const
  {
    switch (m_type)
    {
      case type::boolean:
        return m_value_boolean ? U"true" : U"false";

      case type::number:
        return number_to_string(*m_value_number);

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

  static std::u32string vector_to_source(const std::vector<value>& elements)
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

  static std::u32string record_to_source(
    const std::unordered_map<std::u32string, value>& properties
  )
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

  std::u32string value::to_source() const
  {
    switch (m_type)
    {
      case type::boolean:
        return m_value_boolean ? U"true" : U"false";

      case type::number:
        return number_to_string(*m_value_number);

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

  std::ostream& operator<<(std::ostream& out, enum value::type type)
  {
    out << peelo::unicode::encoding::utf8::encode(
      value::type_description(type)
    );

    return out;
  }

  std::ostream& operator<<(std::ostream& out, const class value& value)
  {
    out << peelo::unicode::encoding::utf8::encode(value.to_string());

    return out;
  }
}
