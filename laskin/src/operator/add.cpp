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
#include "laskin/error.hpp"
#include "laskin/value.hpp"

namespace laskin
{
  static value
  add_month(month a, const value& b)
  {
    long delta;

    if (b.as_number().measurement_unit())
    {
      throw error(
        error::type::type,
        U"Cannot add number to month."
      );
    } else {
      delta = long(b);
    }

    return a + delta;
  }

  static value
  add_weekday(weekday a, const value& b)
  {
    long delta;

    if (const auto& unit = b.as_number().measurement_unit())
    {
      if (!unit->symbol.compare("d"))
      {
        delta = long(b);
      } else {
        throw error(
          error::type::type,
          U"Cannot add number to weekday."
        );
      }
    } else {
      delta = long(b);
    }

    return a + delta;
  }

  static value
  add_date(const date& a, const value& b)
  {
    long delta;

    if (const auto& unit = b.as_number().measurement_unit())
    {
      if (!unit->symbol.compare("d"))
      {
        delta = long(b);
      } else {
        throw error(
          error::type::type,
          U"Cannot add number to date."
        );
      }
    } else {
      delta = long(b);
    }

    return a + delta;
  }

  static value
  add_time(const time& a, const value& b)
  {
    using peelo::chrono::duration;

    long delta;

    if (const auto& unit = b.as_number().measurement_unit())
    {
      duration::value_type multiplier;

      if (!unit->symbol.compare("s"))
      {
        multiplier = 1;
      }
      else if (!unit->symbol.compare("min"))
      {
        multiplier = duration::minutes_per_hour;
      }
      else if (!unit->symbol.compare("h"))
      {
        multiplier = duration::seconds_per_hour;
      }
      else if (!unit->symbol.compare("d"))
      {
        multiplier = duration::seconds_per_day;
      } else {
        throw error(
          error::type::type,
          U"Cannot add number to time."
        );
      }
      delta = long(b) * multiplier;
    } else {
      delta = long(b);
    }

    return a + delta;
  }

  value
  value::add(const value& that) const
  {
    try
    {
      if (that.is(m_type))
      {
        switch (m_type)
        {
          case type::number:
            return *m_value_number + *that.m_value_number;

          case type::vector:
            return *m_value_vector + *that.m_value_vector;

          case type::record:
            return *m_value_record + *that.m_value_record;

          case type::string:
            return *m_value_string + *that.m_value_string;

          default:
            break;
        }
      }
      else if (that.is(type::number))
      {
        switch (m_type)
        {
          case type::month:
            return add_month(m_value_month, that);

          case type::weekday:
            return add_weekday(m_value_weekday, that);

          case type::date:
            return add_date(*m_value_date, that);

          case type::time:
            return add_time(*m_value_time, that);

          case type::vector:
            return *m_value_vector + that;

          default:
            break;
        }
      }
    }
    catch (const number::unit_error& e)
    {
      throw error(error::type::unit, e.what());
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
