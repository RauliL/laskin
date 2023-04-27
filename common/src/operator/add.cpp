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
#include "laskin/error.hpp"
#include "laskin/value.hpp"

namespace laskin
{
  static value add_number(const number& a, const number& b)
  {
    return value::make_number(a + b);
  }

  static value add_vector(const std::vector<value>& a,
                          const std::vector<value>& b)
  {
    const auto size = a.size();
    std::vector<value> result;

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    result.reserve(size);
    for (std::vector<value>::size_type i = 0; i < size; ++i)
    {
      result.push_back(a[i] + b[i]);
    }

    return value::make_vector(result);
  }

  static value add_record(
    const std::unordered_map<std::u32string, value>& a,
    const std::unordered_map<std::u32string, value>& b
  )
  {
    std::unordered_map<std::u32string, value> result(a);

    for (const auto& property : b)
    {
      result[property.first] = property.second;
    }

    return value::make_record(result);
  }

  static value add_number_to_vector(
    const std::vector<value>& a,
    const value& b
  )
  {
    const auto size = a.size();
    std::vector<value> result;

    result.reserve(size);
    for (const auto& value : a)
    {
      result.push_back(value + b);
    }

    return value::make_vector(result);
  }

  static value add_string(const std::u32string& a, const std::u32string& b)
  {
    return value::make_string(a + b);
  }

  static value add_month(peelo::chrono::month a, const number& b)
  {
    long delta;

    if (b.measurement_unit())
    {
      throw error(
        error::type::type,
        U"Cannot add number to month."
      );
    } else {
      delta = b.to_long();
    }

    return value::make_month(a + delta);
  }

  static value add_weekday(peelo::chrono::weekday a, const number& b)
  {
    long delta;

    if (const auto& unit = b.measurement_unit())
    {
      if (!unit->symbol().compare(U"d"))
      {
        delta = b.to_long();
      } else {
        throw error(
          error::type::type,
          U"Cannot add number to weekday."
        );
      }
    } else {
      delta = b.to_long();
    }

    return value::make_weekday(a + delta);
  }

  static value add_date(const peelo::chrono::date& a, const number& b)
  {
    long delta;

    if (const auto& unit = b.measurement_unit())
    {
      if (!unit->symbol().compare(U"d"))
      {
        delta = b.to_long();
      } else {
        throw error(
          error::type::type,
          U"Cannot add number to date."
        );
      }
    } else {
      delta = b.to_long();
    }

    return value::make_date(a + delta);
  }

  static value add_time(const peelo::chrono::time& a, const number& b)
  {
    long delta;

    if (const auto& unit = b.measurement_unit())
    {
      const auto& symbol = unit->symbol();

      if (!symbol.compare(U"s"))
      {
        delta = b.to_long();
      }
      else if (!symbol.compare(U"min"))
      {
        delta = b.to_long() * peelo::chrono::duration::minutes_per_hour;
      }
      else if (!symbol.compare(U"h"))
      {
        delta = b.to_long() * peelo::chrono::duration::seconds_per_hour;
      }
      else if (!symbol.compare(U"d"))
      {
        delta = b.to_long() * peelo::chrono::duration::seconds_per_day;
      } else {
        throw error(
          error::type::type,
          U"Cannot add number to time."
        );
      }
    } else {
      delta = b.to_long();
    }

    return value::make_time(a + delta);
  }

  value value::add(const value& that) const
  {
    if (that.is(m_type))
    {
      switch (m_type)
      {
        case type::number:
          return add_number(*m_value_number, *that.m_value_number);

        case type::vector:
          return add_vector(*m_value_vector, *that.m_value_vector);

        case type::record:
          return add_record(*m_value_record, *that.m_value_record);

        case type::string:
          return add_string(*m_value_string, *that.m_value_string);

        default:
          break;
      }
    }
    else if (that.is(type::number))
    {
      switch (m_type)
      {
        case type::month:
          return add_month(m_value_month, *that.m_value_number);

        case type::weekday:
          return add_weekday(m_value_weekday, *that.m_value_number);

        case type::date:
          return add_date(*m_value_date, *that.m_value_number);

        case type::time:
          return add_time(*m_value_time, *that.m_value_number);

        case type::vector:
          return add_number_to_vector(*m_value_vector, that);

        default:
          break;
      }
    }

    throw error(
      error::type::type,
      U"Cannot add " +
      type_description(that.m_type) +
      U" to " +
      type_description(m_type)
    );
  }
}
