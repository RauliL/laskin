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
  static int compare_number(const number& a, const number& b)
  {
    return a > b ? 1 : a < b ? -1 : 0;
  }

  static int compare_string(const std::u32string& a, const std::u32string& b)
  {
    return a.compare(b);
  }

  static int compare_vector(const std::vector<value>& a,
                            const std::vector<value>& b)
  {
    const auto size_a = a.size();
    const auto size_b = b.size();
    const auto min_size = std::min(size_a, size_b);

    for (std::vector<value>::size_type i = 0; i < min_size; ++i)
    {
      const auto cmp = a[i].compare(b[i]);

      if (cmp != 0)
      {
        return cmp;
      }
    }

    return size_a > size_b ? 1 : size_a < size_b ? -1 : 0;
  }

  static int compare_month(peelo::month a, peelo::month b)
  {
    return a > b ? 1 : a < b ? -1 : 0;
  }

  int value::compare(const value& that) const
  {
    if (that.is(m_type))
    {
      switch (m_type)
      {
        case type::number:
          return compare_number(*m_value_number, *that.m_value_number);

        case type::string:
          return compare_string(*m_value_string, *that.m_value_string);

        case type::vector:
          return compare_vector(*m_value_vector, *that.m_value_vector);

        case type::month:
          return compare_month(m_value_month, that.m_value_month);

        default:
          break;
      }
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
