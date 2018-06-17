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

#include <peelo/unicode.hpp>

#include "laskin/error.hpp"
#include "laskin/quote.hpp"

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

    instance.m_type = type_number;
    instance.m_value_number = new number(value);

    return instance;
  }

  value value::make_number(const std::u32string& input)
  {
    class value instance;

    instance.m_type = type_number;
    instance.m_value_number = new number(number::from_string(input));

    return instance;
  }

  value value::make_vector(const std::vector<value>& elements)
  {
    class value instance;

    instance.m_type = type_vector;
    instance.m_value_vector = new std::vector<value>(elements);

    return instance;
  }

  value value::make_quote(const class quote& quote)
  {
    class value instance;

    instance.m_type = type_quote;
    instance.m_value_quote = new class quote(quote);

    return instance;
  }

  value::value()
    : m_type(type_boolean)
    , m_value_boolean(false) {}

  value::value(const value& that)
    : m_type(that.m_type)
  {
    switch (m_type)
    {
      case type_boolean:
        m_value_boolean = that.m_value_boolean;
        break;

      case type_number:
        m_value_number = new number(*that.m_value_number);
        break;

      case type_vector:
        m_value_vector = new std::vector<value>(*that.m_value_vector);
        break;

      case type_quote:
        m_value_quote = new quote(*that.m_value_quote);
        break;
    }
  }

  value::value(value&& that)
    : m_type(that.m_type)
  {
    switch (m_type)
    {
      case type_boolean:
        m_value_boolean = that.m_value_boolean;
        break;

      case type_number:
        m_value_number = that.m_value_number;
        break;

      case type_vector:
        m_value_vector = that.m_value_vector;
        break;

      case type_quote:
        m_value_quote = that.m_value_quote;
        break;
    }
    that.m_type = type_boolean;
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
        case type_boolean:
          m_value_boolean = that.m_value_boolean;
          break;

        case type_number:
          m_value_number = new number(*that.m_value_number);
          break;

        case type_vector:
          m_value_vector = new std::vector<value>(*that.m_value_vector);
          break;

        case type_quote:
          m_value_quote = new quote(*that.m_value_quote);
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
        case type_boolean:
          m_value_boolean = that.m_value_boolean;
          break;

        case type_number:
          m_value_number = that.m_value_number;
          break;

        case type_vector:
          m_value_vector = that.m_value_vector;
          break;

        case type_quote:
          m_value_quote = that.m_value_quote;
          break;
      }
      that.m_type = type_boolean;
      that.m_value_boolean = false;
    }

    return *this;
  }

  std::u32string value::type_description(enum type type)
  {
    switch (type)
    {
      case type_boolean:
        return U"boolean";

      case type_number:
        return U"number";

      case type_vector:
        return U"vector";

      case type_quote:
        return U"quote";
    }
  }

  void value::reset()
  {
    switch (m_type)
    {
      case type_number:
        delete m_value_number;
        break;

      case type_vector:
        delete m_value_vector;
        break;

      case type_quote:
        delete m_value_quote;
        break;
    }

    m_type = type_boolean;
    m_value_boolean = false;
  }

  bool value::as_boolean() const
  {
    if (!is(type_boolean))
    {
      throw error(
        error::type_type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting boolean."
      );
    }

    return m_value_boolean;
  }

  const number& value::as_number() const
  {
    if (!is(type_number))
    {
      throw error(
        error::type_type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting number."
      );
    }

    return *m_value_number;
  }

  const std::vector<value>& value::as_vector() const
  {
    if (!is(type_vector))
    {
      throw error(
        error::type_type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting vector."
      );
    }

    return *m_value_vector;
  }

  const quote& value::as_quote() const
  {
    if (!is(type_quote))
    {
      throw error(
        error::type_type,
        U"Unexpected " +
        type_description(m_type) +
        U"; Was excepting quote."
      );
    }

    return *m_value_quote;
  }

  static std::u32string number_to_string(const number& value)
  {
    return value.to_string();
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

  std::u32string value::to_string() const
  {
    switch (m_type)
    {
      case type_boolean:
        return m_value_boolean ? U"true" : U"false";

      case type_number:
        return number_to_string(*m_value_number);

      case type_vector:
        return vector_to_string(*m_value_vector);

      case type_quote:
        return m_value_quote->to_source();
    }
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

  std::u32string value::to_source() const
  {
    switch (m_type)
    {
      case type_boolean:
        return m_value_boolean ? U"true" : U"false";

      case type_number:
        return number_to_string(*m_value_number);

      case type_vector:
        return vector_to_source(*m_value_vector);

      case type_quote:
        return m_value_quote->to_source();
    }
  }

  static inline int compare_number(const number& a, const number& b)
  {
    return a > b ? 1 : a < b ? -1 : 0;
  }

  int value::compare(const value& that) const
  {
    if (!that.is(m_type))
    {
      throw error(
        error::type_type,
        U"Cannot compare " +
        type_description(m_type) +
        U" with " +
        type_description(that.m_type)
      );
    }

    switch (m_type)
    {
      case type_number:
        return compare_number(*m_value_number, *that.m_value_number);
    }

    return 0; // TODO
  }

  std::ostream& operator<<(std::ostream& out, enum value::type type)
  {
    out << peelo::unicode::utf8::encode(value::type_description(type));

    return out;
  }

  std::ostream& operator<<(std::ostream& out, const class value& value)
  {
    out << peelo::unicode::utf8::encode(value.to_string());

    return out;
  }
}
