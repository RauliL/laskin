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
  static int
  compare_vector(const vector& a, const vector& b)
  {
    const auto size_a = a.size();
    const auto size_b = b.size();
    const auto min_size = std::min(size_a, size_b);

    for (vector::size_type i = 0; i < min_size; ++i)
    {
      const auto cmp = a[i].compare(b[i]);

      if (cmp != 0)
      {
        return cmp;
      }
    }

    return size_a > size_b ? 1 : size_a < size_b ? -1 : 0;
  }

  static inline int
  compare_month(month a, month b)
  {
    return a > b ? 1 : a < b ? -1 : 0;
  }

  static inline int
  compare_weekday(weekday a, weekday b)
  {
    return a > b ? 1 : a < b ? -1 : 0;
  }

  int
  value::compare(const value& that) const
  {
    try
    {
      if (that.is(m_type))
      {
        switch (m_type)
        {
          case type::number:
            return m_value_number->compare(*that.m_value_number);

          case type::string:
            return m_value_string->compare(*that.m_value_string);

          case type::vector:
            return compare_vector(*m_value_vector, *that.m_value_vector);

          case type::month:
            return compare_month(m_value_month, that.m_value_month);

          case type::weekday:
            return compare_weekday(m_value_weekday, that.m_value_weekday);

          case type::date:
            return m_value_date->compare(*that.m_value_date);

          case type::time:
            return m_value_time->compare(*that.m_value_time);

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
      U"Cannot compare " +
      type_description(that.m_type) +
      U" with " +
      type_description(m_type)
    );
  }
}
