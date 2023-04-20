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
#include "laskin/utils.hpp"
#include "laskin/value.hpp"

namespace laskin
{
  static value substract_number(const number& a, const number& b)
  {
    return value::make_number(a - b);
  }

  static value substract_vector(const std::vector<value>& a,
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
      result.push_back(a[i] - b[i]);
    }

    return value::make_vector(result);
  }

  static value substract_date(const peelo::chrono::date& a,
                              const peelo::chrono::date& b)
  {
    return value::make_number((a - b).days(), unit::day);
  }

  static value substract_time(const peelo::chrono::time& a,
                              const peelo::chrono::time& b)
  {
    return value::make_number(
      utils::time_as_seconds(a) - utils::time_as_seconds(b),
      unit::second
    );
  }

  value value::substract(const value& that) const
  {
    if (that.is(m_type))
    {
      switch (m_type)
      {
        case type::number:
          return substract_number(*m_value_number, *that.m_value_number);

        case type::vector:
          return substract_vector(*m_value_vector, *that.m_value_vector);

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
          // TODO: Make sure there is no measurement units.
          return make_month(m_value_month - that.m_value_number->to_long());

        case type::weekday:
          // TODO: Make sure there is no measurement units.
          return make_weekday(m_value_weekday - that.m_value_number->to_long());

        case type::date:
          // TODO: Acknowledge measurement units.
          return make_date(*m_value_date - that.m_value_number->to_long());

        case type::time:
          // TODO: Acknowledge measurement units.
          return make_time(*m_value_time - that.m_value_number->to_long());

        default:
          break;
      }
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

