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
  bool
  value::equals(const value& that) const
  {
    if (that.is(m_type))
    {
      switch (m_type)
      {
        case type::boolean:
          return m_value_boolean == that.m_value_boolean;

        case type::number:
          return *m_value_number == *that.m_value_number;

        case type::vector:
          return *m_value_vector == *that.m_value_vector;

        case type::record:
          return *m_value_record == *that.m_value_record;

        case type::string:
          return *m_value_string == *that.m_value_string;

        case type::month:
          return m_value_month == that.m_value_month;

        case type::weekday:
          return m_value_weekday == that.m_value_weekday;

        case type::date:
          return *m_value_date == *that.m_value_date;

        case type::time:
          return *m_value_time == *that.m_value_time;

        case type::quote:
          return *m_value_quote == *that.m_value_quote;
      }
    }

    return false;
  }
}
