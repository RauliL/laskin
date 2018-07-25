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

namespace laskin
{
  node::node(int line, int column)
    : m_line(line)
    , m_column(column) {}

  node::literal::literal(const class value& value, int line, int column)
    : node(line, column)
    , m_value(value) {}

  void node::literal::exec(class context& context, std::ostream& out) const
  {
    context.data().push_back(m_value);
  }

  value node::literal::eval(class context& context, std::ostream& out) const
  {
    return m_value;
  }

  std::u32string node::literal::to_source() const
  {
    return m_value.to_source();
  }

  node::vector_literal::vector_literal(const container_type& elements,
                                       int line,
                                       int column)
    : node(line, column)
    , m_elements(elements) {}

  void node::vector_literal::exec(class context& context,
                                  std::ostream& out) const
  {
    context.data().push_back(eval(context, out));
  }

  value node::vector_literal::eval(class context& context,
                                   std::ostream& out) const
  {
    std::vector<value> container;

    container.reserve(m_elements.size());
    for (const auto& element : m_elements)
    {
      container.push_back(element->eval(context, out));
    }

    return value::make_vector(container);
  }

  std::u32string node::vector_literal::to_source() const
  {
    std::u32string result;
    bool first = true;

    result.append(1, U'[');
    for (const auto& element : m_elements)
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
    result.append(1, U']');

    return result;
  }

  node::symbol::symbol(const std::u32string& id, int line, int column)
    : node(line, column)
    , m_id(id) {}

  void node::symbol::exec(class context& context, std::ostream& out) const
  {
    auto& data = context.data();
    const auto& dictionary = context.dictionary();

    if (!data.empty())
    {
      const auto& value = data.back();
      const auto id = value::type_description(value.type()) + U':' + m_id;
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

    {
      const auto word = dictionary.find(m_id);

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

    if (is_number(m_id))
    {
      data.push_back(value::make_number(m_id));
      return;
    }
    else if (is_date(m_id))
    {
      data.push_back(value::make_date(m_id));
      return;
    }
    else if (is_time(m_id))
    {
      data.push_back(value::make_time(m_id));
      return;
    }

    throw error(
      error::type::name,
      U"Unrecognized symbol: `" + m_id + U"'",
      line(),
      column()
    );
  }

  value node::symbol::eval(class context& context, std::ostream&) const
  {
    if (!m_id.compare(U"true"))
    {
      return value::make_boolean(true);
    }
    else if (!m_id.compare(U"false"))
    {
      return value::make_boolean(false);
    }
    else if (!m_id.compare(U"drop"))
    {
      return context.pop();
    }
    else if (is_number(m_id))
    {
      return value::make_number(m_id);
    }
    else if (is_date(m_id))
    {
      return value::make_date(m_id);
    }
    else if (is_time(m_id))
    {
      return value::make_time(m_id);
    }

    throw error(
      error::type::name,
      U"Unable to evaluate `" + m_id + U"' as expression.",
      line(),
      column()
    );
  }

  std::u32string node::symbol::to_source() const
  {
    return m_id;
  }

  node::definition::definition(const std::u32string& id, int line, int column)
    : node(line, column)
    , m_id(id) {}

  void node::definition::exec(class context& context, std::ostream&) const
  {
    context.dictionary()[m_id] = context.pop();
  }

  value node::definition::eval(context&, std::ostream&) const
  {
    throw error(
      error::type::syntax,
      U"Unable to evaluate definition of `" + m_id + U"' as expression.",
      line(),
      column()
    );
  }

  std::u32string node::definition::to_source() const
  {
    return U"-> " + m_id;
  }
}
