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
  std::u32string
  node::type_description(enum type type)
  {
    switch (type)
    {
      case type::definition:
        return U"definition";

      case type::literal:
        return U"literal";

      case type::record:
        return U"record";

      case type::symbol:
        return U"symbol";

      case type::vector:
        return U"vector";
    }

    return U"unknown";
  }

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
  node::vector::exec(
    class context& context,
    std::ostream* out
  ) const
  {
    context.data.push_back(eval(context, out));
  }

  value
  node::vector::eval(
    class context& context,
    std::ostream* out
  ) const
  {
    laskin::vector container;

    container.reserve(elements.size());
    for (const auto& element : elements)
    {
      container.push_back(element->eval(context, out));
    }

    return container;
  }

  bool
  node::vector::equals(const std::shared_ptr<node>& that) const
  {
    if (that && that->type() == type::vector)
    {
      const auto t = std::static_pointer_cast<vector>(that);
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
  node::vector::to_source() const
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
  node::record::exec(
    class context& context,
    std::ostream* out
  ) const
  {
    context.data.push_back(eval(context, out));
  }

  value
  node::record::eval(
    class context& context,
    std::ostream* out
  ) const
  {
    laskin::record resolved_properties;

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
  node::record::equals(const std::shared_ptr<node>& that) const
  {
    if (that && that->type() == type::record)
    {
      const auto t = std::static_pointer_cast<record>(that);

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
  node::record::to_source() const
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
