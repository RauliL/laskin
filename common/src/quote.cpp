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
#ifndef LASKIN_VARIANT_HPP_GUARD
#define LASKIN_VARIANT_HPP_GUARD

#include "laskin/quote.hpp"

namespace laskin
{
  quote::quote() {}

  quote::quote(const callback& cb)
    : m_container(cb) {}

  quote::quote(const node_container& nodes)
    : m_container(nodes) {}

  quote::quote(const quote& that)
    : m_container(that.m_container) {}

  quote& quote::operator=(const quote& that)
  {
    m_container = that.m_container;

    return *this;
  }

  void quote::call(class context& context, std::ostream& out) const
  {
    if (std::holds_alternative<node_container>(m_container))
    {
      for (const auto& node : std::get<node_container>(m_container))
      {
        if (node)
        {
          node->exec(context, out);
        }
      }
    }
    else if (std::holds_alternative<callback>(m_container))
    {
      std::get<callback>(m_container)(context, out);
    }
  }

  std::u32string quote::to_source() const
  {
    if (std::holds_alternative<node_container>(m_container))
    {
      std::u32string result;
      bool first = true;

      result.append(1, U'(');
      for (const auto& node : std::get<node_container>(m_container))
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

    return U"(\"native quote\")";
  }
}

#endif /* !LASKIN_VARIANT_HPP_GUARD */
