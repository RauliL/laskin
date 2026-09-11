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
#include "laskin/ast.hpp"
#include "laskin/error.hpp"
#include "laskin/quote.hpp"

namespace laskin
{
  void
  call(
    const quote& q,
    context& c,
    std::ostream* out
  )
  {
    if (std::holds_alternative<scripted_quote>(q))
    {
      for (const auto& node : std::get<scripted_quote>(q))
      {
        if (node)
        {
          try
          {
            node->exec(c, out);
          }
          catch (const error& e)
          {
            throw error(e.type, e.message, node->position);
          }
        }
      }
    }
    else if (std::holds_alternative<native_quote>(q))
    {
      std::get<native_quote>(q)(c, out);
    }
  }

  bool
  equals(const quote& a, const quote& b)
  {
    if (std::holds_alternative<scripted_quote>(a))
    {
      const auto& aa = std::get<scripted_quote>(a);

      if (std::holds_alternative<scripted_quote>(b))
      {
        const auto& bb = std::get<scripted_quote>(b);
        const auto size = aa.size();

        if (bb.size() != size)
        {
          return false;
        }
        for (scripted_quote::size_type i = 0; i < size; ++i)
        {
          if (!node::equals(aa[i], bb[i]))
          {
            return false;
          }
        }

        return true;
      }
      else if (!std::holds_alternative<native_quote>(b))
      {
        return aa.empty();
      }

      return false;
    }

    if (std::holds_alternative<native_quote>(a))
    {
      // It's almost impossible to test equality between two `std::function`
      // instances. While some clever hacks to retrieve the memory address
      // exist, I could not get them working reliably. For native quotes the
      // result will always be `false` until I find working solution. Sorry.
      return false;
    }

    return !std::holds_alternative<scripted_quote>(b) &&
      !std::holds_alternative<native_quote>(b);
  }

  std::u32string
  to_source(const quote& q)
  {
    if (std::holds_alternative<scripted_quote>(q))
    {
      std::u32string result;
      bool first = true;

      result.append(1, U'(');
      for (const auto& node : std::get<scripted_quote>(q))
      {
        if (first)
        {
          first = false;
        } else {
          result.append(1, U' ');
        }
        if (node)
        {
          result.append(node->to_source());
        }
      }
      result.append(1, U')');

      return result;
    }
    else if (std::holds_alternative<native_quote>(q))
    {
      return U"(\"native quote\")";
    }

    return U"()";
  }
}
