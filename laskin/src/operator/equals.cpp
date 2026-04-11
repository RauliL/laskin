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
#include "laskin/quote.hpp"
#include "laskin/value.hpp"

namespace laskin
{
  static inline bool
  equals_boolean(bool a, bool b)
  {
    return a == b;
  }

  static inline bool
  equals_number(const number& a, const number& b)
  {
    return a == b;
  }

  static bool
  equals_vector(const vector& a, const vector& b)
  {
    const auto size = a.size();

    if (size != b.size())
    {
      return false;
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      if (a[i] != b[i])
      {
        return false;
      }
    }

    return true;
  }

  static bool
  equals_record(const record& a, const record& b)
  {
    if (a.size() != b.size())
    {
      return false;
    }
    for (const auto& property : a)
    {
      const auto i = b.find(property.first);

      if (i == std::end(b))
      {
        return false;
      }
      else if (!i->second.equals(property.second))
      {
        return false;
      }
    }

    return true;
  }

  static inline bool
  equals_string(const std::u32string& a, const std::u32string& b)
  {
    return !a.compare(b);
  }

  static inline bool
  equals_month(month a, month b)
  {
    return a == b;
  }

  static inline bool
  equals_weekday(weekday a, weekday b)
  {
    return a == b;
  }

  static inline bool
  equals_date(const date& a, const date& b)
  {
    return a == b;
  }

  static inline bool
  equals_time(const time& a, const time& b)
  {
    return a == b;
  }

  static inline bool
  equals_quote(const quote& a, const quote& b)
  {
    return a == b;
  }

  bool
  value::equals(const value& that) const
  {
    if (that.is(m_type))
    {
      switch (m_type)
      {
        case type::boolean:
          return equals_boolean(m_value_boolean, that.m_value_boolean);

        case type::number:
          return equals_number(*m_value_number, *that.m_value_number);

        case type::vector:
          return equals_vector(*m_value_vector, *that.m_value_vector);

        case type::record:
          return equals_record(*m_value_record, *that.m_value_record);

        case type::string:
          return equals_string(*m_value_string, *that.m_value_string);

        case type::month:
          return equals_month(m_value_month, that.m_value_month);

        case type::weekday:
          return equals_weekday(m_value_weekday, that.m_value_weekday);

        case type::date:
          return equals_date(*m_value_date, *that.m_value_date);

        case type::time:
          return equals_time(*m_value_time, *that.m_value_time);

        case type::quote:
          return equals_quote(*m_value_quote, *that.m_value_quote);

        default:
          break;
      }
    }

    return false;
  }
}
