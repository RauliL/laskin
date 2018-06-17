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
#include "laskin/value.hpp"

namespace laskin
{
  static bool equals_boolean(bool a, bool b)
  {
    return a == b;
  }

  static bool equals_number(const number& a, const number& b)
  {
    return a == b;
  }

  static bool equals_vector(const std::vector<value>& a,
                            const std::vector<value>& b)
  {
    const auto size = a.size();

    if (size != b.size())
    {
      return false;
    }
    for (std::vector<value>::size_type i = 0; i < size; ++i)
    {
      if (a[i] != b[i])
      {
        return false;
      }
    }

    return true;
  }

  bool value::equals(const value& that) const
  {
    if (that.is(m_type))
    {
      switch (m_type)
      {
        case type_boolean:
          return equals_boolean(m_value_boolean, that.m_value_boolean);

        case type_number:
          return equals_number(*m_value_number, *that.m_value_number);

        case type_vector:
          return equals_vector(*m_value_vector, *that.m_value_vector);

        // TODO: Quote equality test.

        default:
          break;
      }
    }

    return false;
  }
}
