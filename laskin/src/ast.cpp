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
#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/utils.hpp"

namespace laskin
{
  void
  node::literal::exec(
    class context& context,
    std::ostream*
  ) const
  {
    context.data.push_back(value);
  }

  bool
  node::literal::equals(const std::shared_ptr<node>& that) const
  {
    if (that && that->type() == type::literal)
    {
      return value == std::static_pointer_cast<literal>(that)->value;
    }

    return false;
  }

  void
  node::vector_literal::exec(
    class context& context,
    std::ostream* out
  ) const
  {
    context.data.push_back(eval(context, out));
  }

  value
  node::vector_literal::eval(
    class context& context,
    std::ostream* out
  ) const
  {
    vector container;

    container.reserve(elements.size());
    for (const auto& element : elements)
    {
      container.push_back(element->eval(context, out));
    }

    return container;
  }

  bool
  node::vector_literal::equals(const std::shared_ptr<node>& that) const
  {
    if (that && that->type() == type::vector_literal)
    {
      const auto t = std::static_pointer_cast<vector_literal>(that);
      const auto size = elements.size();

      if (t->elements.size() != size)
      {
        return false;
      }
      for (container_type::size_type i = 0; i < size; ++i)
      {
        if (!node::equals(elements[i], t->elements[i]))
        {
          return false;
        }
      }

      return true;
    }

    return false;
  }

  std::u32string
  node::vector_literal::to_source() const
  {
    std::u32string result(1, U'[');
    bool first = true;

    for (const auto& element : elements)
    {
      if (first)
      {
        first = false;
      } else {
        result.append(U", ");
      }
      if (element)
      {
        result.append(element->to_source());
      }
    }

    return result.append(1, U']');
  }

  void
  node::record_literal::exec(
    class context& context,
    std::ostream* out
  ) const
  {
    context.data.push_back(eval(context, out));
  }

  value
  node::record_literal::eval(
    class context& context,
    std::ostream* out
  ) const
  {
    record resolved_properties;

    for (const auto& property : properties)
    {
      resolved_properties[property.first] = property.second->eval(
        context,
        out
      );
    }

    return resolved_properties;
  }

  bool
  node::record_literal::equals(const std::shared_ptr<node>& that) const
  {
    if (that && that->type() == type::record_literal)
    {
      const auto t = std::static_pointer_cast<record_literal>(that);

      if (properties.size() != t->properties.size())
      {
        return false;
      }
      for (const auto& property : properties)
      {
        const auto it = t->properties.find(property.first);

        if (
          it == std::end(t->properties) ||
          !node::equals(property.second, it->second)
        )
        {
          return false;
        }
      }

      return true;
    }

    return false;
  }

  std::u32string
  node::record_literal::to_source() const
  {
    std::u32string result(1, U'{');
    bool first = true;

    for (const auto& property : properties)
    {
      if (first)
      {
        first = false;
      } else {
        result.append(U", ");
      }
      result
        .append(utils::escape_string(property.first))
        .append(U": ")
        .append(property.second->to_source());
    }

    return result.append(1, U'}');
  }

  void
  node::symbol::exec(
    class context& context,
    std::ostream* out
  ) const
  {
    context.lookup(id, out, position);
  }

  value
  node::symbol::eval(
    class context& context,
    std::ostream*
  ) const
  {
    return context.eval(id, position);
  }

  bool
  node::symbol::equals(const std::shared_ptr<node>& that) const
  {
    if (that && that->type() == type::symbol)
    {
      return id == std::static_pointer_cast<symbol>(that)->id;
    }

    return false;
  }

  void
  node::definition::exec(
    class context& context,
    std::ostream*
  ) const
  {
    context.dictionary[id] = context.pop();
  }

  value
  node::definition::eval(
    context&,
    std::ostream*
  ) const
  {
    throw error(
      error::type::syntax,
      U"Unable to evaluate definition of `" + id + U"' as expression.",
      position
    );
  }

  bool
  node::definition::equals(const std::shared_ptr<node>& that) const
  {
    if (that && that->type() == type::definition)
    {
      return id == std::static_pointer_cast<definition>(that)->id;
    }

    return false;
  }
}
