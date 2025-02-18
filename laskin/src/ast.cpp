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
#include "laskin/chrono.hpp"
#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/quote.hpp"
#include "laskin/utils.hpp"

namespace laskin
{
  void node::literal::exec(class context& context, std::ostream& out) const
  {
    context.data().push_back(value);
  }

  void node::vector_literal::exec(class context& context,
                                  std::ostream& out) const
  {
    context.data().push_back(eval(context, out));
  }

  value node::vector_literal::eval(class context& context,
                                   std::ostream& out) const
  {
    std::vector<value> container;

    container.reserve(elements.size());
    for (const auto& element : elements)
    {
      container.push_back(element->eval(context, out));
    }

    return value::make_vector(container);
  }

  std::u32string node::vector_literal::to_source() const
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

  void node::record_literal::exec(
    class context& context,
    std::ostream& out
  ) const
  {
    context.data().push_back(eval(context, out));
  }

  value node::record_literal::eval(
    class context& context,
    std::ostream& out
  ) const
  {
    tsl::ordered_map<std::u32string, value> resolved_properties;

    for (const auto& property : properties)
    {
      resolved_properties[property.first] = property.second->eval(
        context,
        out
      );
    }

    return value::make_record(resolved_properties);
  }

  std::u32string node::record_literal::to_source() const
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

  void node::symbol::exec(class context& context, std::ostream& out) const
  {
    auto& data = context.data();
    const auto& dictionary = context.dictionary();

    if (!data.empty())
    {
      const auto& value = data.back();
      const auto type_id = value::type_description(value.type()) + U':' + id;
      const auto word = dictionary.find(type_id);

      if (word != std::end(dictionary))
      {
        if (word->second.is(value::type::quote))
        {
          word->second.as_quote().call(context, out);
        } else {
          data.push_back(word->second);
        }
        return;
      }
    }

    {
      const auto word = dictionary.find(id);

      if (word != std::end(dictionary))
      {
        if (word->second.is(value::type::quote))
        {
          word->second.as_quote().call(context, out);
        } else {
          data.push_back(word->second);
        }
        return;
      }
    }

    if (number::is_valid(id))
    {
      data.push_back(value::make_number(id));
      return;
    }
    else if (is_date(id))
    {
      data.push_back(value::make_date(id));
      return;
    }
    else if (is_time(id))
    {
      data.push_back(value::make_time(id));
      return;
    }

    if (const auto& default_callback = context.default_callback())
    {
      if (const auto value = default_callback(id))
      {
        data.push_back(*value);
        return;
      }
    }

    throw error(
      error::type::name,
      U"Unrecognized symbol: `" + id + U"'",
      line,
      column
    );
  }

  value node::symbol::eval(class context& context, std::ostream&) const
  {
    if (!id.compare(U"true"))
    {
      return value::make_boolean(true);
    }
    else if (!id.compare(U"false"))
    {
      return value::make_boolean(false);
    }
    else if (!id.compare(U"drop"))
    {
      return context.pop();
    }
    else if (number::is_valid(id))
    {
      return value::make_number(id);
    }
    else if (is_date(id))
    {
      return value::make_date(id);
    }
    else if (is_time(id))
    {
      return value::make_time(id);
    }
    else if (is_month(id))
    {
      return value::make_month(id);
    }
    else if (is_weekday(id))
    {
      return value::make_weekday(id);
    }

    throw error(
      error::type::name,
      U"Unable to evaluate `" + id + U"' as expression.",
      line,
      column
    );
  }

  void node::definition::exec(class context& context, std::ostream&) const
  {
    context.dictionary()[id] = context.pop();
  }

  value node::definition::eval(context&, std::ostream&) const
  {
    throw error(
      error::type::syntax,
      U"Unable to evaluate definition of `" + id + U"' as expression.",
      line,
      column
    );
  }
}
