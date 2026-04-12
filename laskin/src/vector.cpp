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
  vector
  operator+(const vector& a, const vector& b)
  {
    const auto size = a.size();
    vector result(a);

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      result[i] += b[i];
    }

    return result;
  }

  vector
  operator+(const vector& a, const value& b)
  {
    const auto size = a.size();
    vector result;

    result.reserve(size);
    for (const auto& value : a)
    {
      result.push_back(value + b);
    }

    return result;
  }

  vector
  operator-(const vector& a, const vector& b)
  {
    const auto size = a.size();
    vector result(a);

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      result[i] -= b[i];
    }

    return result;
  }

  vector
  operator-(const vector& a, const value& b)
  {
    const auto size = a.size();
    vector result;

    result.reserve(size);
    for (const auto& value : a)
    {
      result.push_back(value - b);
    }

    return result;
  }

  vector
  operator*(const vector& a, const vector& b)
  {
    const auto size = a.size();
    vector result(a);

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      result[i] *= b[i];
    }

    return result;
  }

  vector
  operator*(const vector& a, const value& b)
  {
    const auto size = a.size();
    vector result;

    result.reserve(size);
    for (const auto& value : a)
    {
      result.push_back(value * b);
    }

    return result;
  }

  vector
  operator/(const vector& a, const vector& b)
  {
    const auto size = a.size();
    vector result(a);

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      result[i] /= b[i];
    }

    return result;
  }

  vector
  operator/(const vector& a, const value& b)
  {
    const auto size = a.size();
    vector result;

    result.reserve(size);
    for (const auto& value : a)
    {
      result.push_back(value / b);
    }

    return result;
  }

  vector
  operator%(const vector& a, const vector& b)
  {
    const auto size = a.size();
    vector result(a);

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      result[i] %= b[i];
    }

    return result;
  }

  vector
  operator%(const vector& a, const value& b)
  {
    const auto size = a.size();
    vector result;

    result.reserve(size);
    for (const auto& value : a)
    {
      result.push_back(value % b);
    }

    return result;
  }

  vector&
  operator+=(vector& a, const vector& b)
  {
    const auto size = a.size();

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] += b[i];
    }

    return a;
  }

  vector&
  operator+=(vector& a, const value& b)
  {
    const auto size = a.size();

    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] += b;
    }

    return a;
  }

  vector&
  operator-=(vector& a, const vector& b)
  {
    const auto size = a.size();

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] -= b[i];
    }

    return a;
  }

  vector&
  operator-=(vector& a, const value& b)
  {
    const auto size = a.size();

    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] -= b;
    }

    return a;
  }

  vector&
  operator*=(vector& a, const vector& b)
  {
    const auto size = a.size();

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] *= b[i];
    }

    return a;
  }

  vector&
  operator*=(vector& a, const value& b)
  {
    const auto size = a.size();

    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] *= b;
    }

    return a;
  }

  vector&
  operator/=(vector& a, const vector& b)
  {
    const auto size = a.size();

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] /= b[i];
    }

    return a;
  }

  vector&
  operator/=(vector& a, const value& b)
  {
    const auto size = a.size();

    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] /= b;
    }

    return a;
  }

  vector&
  operator%=(vector& a, const vector& b)
  {
    const auto size = a.size();

    if (size != b.size())
    {
      throw error(error::type::range, U"Vector length mismatch.");
    }
    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] %= b[i];
    }

    return a;
  }

  vector&
  operator%=(vector& a, const value& b)
  {
    const auto size = a.size();

    for (vector::size_type i = 0; i < size; ++i)
    {
      a[i] %= b;
    }

    return a;
  }
}
