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
#include "laskin/utils.hpp"
#include "laskin/value.hpp"

namespace laskin
{
  static inline value
  substract_number(const peelo::number& a, const peelo::number& b)
  {
    return value::make_number(a - b);
  }

  static value
  substract_vector(
    const value::vector_container& a,
    const value::vector_container& b
  )
  {
    const auto size = a.size();
    value::vector_container result(a);

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    result.reserve(size);
    for (value::vector_container::size_type i = 0; i < size; ++i)
    {
      result[i] -= b[i];
    }

    return value::make_vector(result);
  }

  static value
  substract_record(
    const value::record_container& a,
    const value::record_container& b
  )
  {
    value::record_container result(a);

    for (const auto& property : b)
    {
      const auto i = result.find(property.first);

      if (i != std::end(result))
      {
        result.erase(i);
      }
    }

    return value::make_record(result);
  }

  static value
  substract_number_from_vector(
    const value::vector_container& a,
    const value& b
  )
  {
    const auto size = a.size();
    value::vector_container result(a);

    for (value::vector_container::size_type i = 0; i < size; ++i)
    {
      result[i] -= b;
    }

    return value::make_vector(result);
  }

  static inline value
  substract_date(const peelo::chrono::date& a, const peelo::chrono::date& b)
  {
    return value::make_number((a - b).days(), peelo::number::unit::day);
  }

  static inline value
  substract_time(const peelo::chrono::time& a, const peelo::chrono::time& b)
  {
    return value::make_number(
      utils::time_as_seconds(a) - utils::time_as_seconds(b),
      peelo::number::unit::second
    );
  }

  static value
  substract_month(peelo::chrono::month a, const peelo::number& b)
  {
    long delta;

    if (b.measurement_unit())
    {
      throw error(
        error::type::type,
        U"Cannot substract number to month."
      );
    } else {
      delta = long(b);
    }

    return value::make_month(a - delta);
  }

  static value
  substract_weekday(peelo::chrono::weekday a, const peelo::number& b)
  {
    long delta;

    if (const auto& unit = b.measurement_unit())
    {
      if (!unit->symbol.compare("d"))
      {
        delta = long(b);
      } else {
        throw error(
          error::type::type,
          U"Cannot substract number to weekday."
        );
      }
    } else {
      delta = long(b);
    }

    return value::make_weekday(a - delta);
  }

  static value
  substract_date(const peelo::chrono::date& a, const peelo::number& b)
  {
    long delta;

    if (const auto& unit = b.measurement_unit())
    {
      if (!unit->symbol.compare("d"))
      {
        delta = long(b);
      } else {
        throw error(
          error::type::type,
          U"Cannot substract number to date."
        );
      }
    } else {
      delta = long(b);
    }

    return value::make_date(a - delta);
  }

  static value
  substract_time(const peelo::chrono::time& a, const peelo::number& b)
  {
    long delta;

    if (const auto& unit = b.measurement_unit())
    {
      if (!unit->symbol.compare("s"))
      {
        delta = long(b);
      }
      else if (!unit->symbol.compare("min"))
      {
        delta = long(b) * peelo::chrono::duration::minutes_per_hour;
      }
      else if (!unit->symbol.compare("h"))
      {
        delta = long(b) * peelo::chrono::duration::seconds_per_hour;
      }
      else if (!unit->symbol.compare("d"))
      {
        delta = long(b) * peelo::chrono::duration::seconds_per_day;
      } else {
        throw error(
          error::type::type,
          U"Cannot substract number to time."
        );
      }
    } else {
      delta = long(b);
    }

    return value::make_time(a - delta);
  }

  value
  value::substract(const value& that) const
  {
    try
    {
      if (that.is(m_type))
      {
        switch (m_type)
        {
          case type::number:
            return substract_number(*m_value_number, *that.m_value_number);

          case type::vector:
            return substract_vector(*m_value_vector, *that.m_value_vector);

          case type::record:
            return substract_record(*m_value_record, *that.m_value_record);

          case type::date:
            return substract_date(*m_value_date, *that.m_value_date);

          case type::time:
            return substract_time(*m_value_time, *that.m_value_time);

          default:
            break;
        }
      }
      else if (that.is(type::number))
      {
        switch (m_type)
        {
          case type::month:
            return substract_month(m_value_month, *that.m_value_number);

          case type::weekday:
            return substract_weekday(m_value_weekday, *that.m_value_number);

          case type::date:
            return substract_date(*m_value_date, *that.m_value_number);

          case type::time:
            return substract_time(*m_value_time, *that.m_value_number);

          case type::vector:
            return substract_number_from_vector(*m_value_vector, that);

          default:
            break;
        }
      }
    }
    catch (const std::underflow_error&)
    {
      throw error(error::type::range, U"Numeric underflow.");
    }
    catch (const std::overflow_error&)
    {
      throw error(error::type::range, U"Numeric overflow.");
    }
    catch (const peelo::number::unit_error& e)
    {
      throw error(error::type::unit, e.what());
    }

    throw error(
      error::type::type,
      U"Cannot substract " +
      type_description(that.m_type) +
      U" from " +
      type_description(m_type)
    );
  }
}

