/*
 * Copyright (c) 2026, Rauli Laine
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
#include <laskin/error.hpp>

#include "./program.hpp"
#include "./transpile.hpp"

namespace laskin2cpp
{
  static void
  transpile_boolean(bool value, class writer& writer)
  {
    writer.print(value ? "true" : "false");
  }

  static void
  transpile_date(const laskin::date& value, class writer& writer)
  {
    writer.print("date(");
    writer.print(std::to_string(value.year()));
    writer.print(", static_cast<month>(");
    writer.print(std::to_string(static_cast<int>(value.month())));
    writer.print("), ");
    writer.print(std::to_string(value.day()));
    writer.print(")");
  }

  static void
  transpile_month(const laskin::month value, class writer& writer)
  {
    writer.print("static_cast<month>(");
    writer.print(std::to_string(static_cast<int>(value)));
    writer.print(")");
  }

  void
  transpile_number(const laskin::number& value, class writer& writer)
  {
    // If the number does not have measurement unit, see if it fits into
    // `double`.
    if (!value.measurement_unit())
    {
      bool fits = true;
      double result;

      try
      {
        result = double(value);
      }
      catch (const std::underflow_error&)
      {
        fits = false;
      }
      catch (const std::overflow_error&)
      {
        fits = false;
      }
      if (fits)
      {
        writer.print("number(");
        writer.print(std::to_string(result));
        writer.print(")");
        return;
      }
    }

    // TODO: Find out more optimal way of converting measurement unit into C++.
    writer.print("value::parse_number(\"");
    writer.print(value.to_string());
    writer.print("\")");
  }

  static void
  transpile_quote(
    const laskin::quote& value,
    class writer& writer,
    const struct options& options
  )
  {
    program subprogram;

    subprogram.compile(value, options);
    writer.println("quote(");
    writer.indent();
    writer.println("[](context& c, std::ostream* out)");
    writer.println("{");
    writer.indent();
    for (const auto& instruction : subprogram.instructions())
    {
      instruction->transpile(writer, options);
    }
    writer.dedent();
    writer.println("}");
    writer.dedent();
    writer.print(")");
  }

  static void
  transpile_record(
    const laskin::record& value,
    class writer& writer,
    const struct options& options
  )
  {
    writer.println("record{");
    writer.indent();
    for (const auto& property : value)
    {
      writer.print("{ ");
      writer.print(writer::escape(property.first));
      writer.print(", ");
      transpile(property.second, writer, options);
      writer.println(" },");
    }
    writer.dedent();
    writer.print("}");
  }

  static void
  transpile_string(const std::u32string& value, class writer& writer)
  {
    writer.print(writer::escape(value));
  }

  static void
  transpile_time(const laskin::time& value, class writer& writer)
  {
    writer.print("time(");
    writer.print(std::to_string(value.hour()));
    writer.print(", ");
    writer.print(std::to_string(value.hour()));
    writer.print(", ");
    writer.print(std::to_string(value.minute()));
    writer.print(")");
  }

  static void
  transpile_vector(
    const laskin::vector& value,
    class writer& writer,
    const struct options& options
  )
  {
    writer.println("vector{");
    writer.indent();
    for (const auto& element : value)
    {
      transpile(element, writer, options);
      writer.println(",");
    }
    writer.dedent();
    writer.print("}");
  }

  static void
  transpile_weekday(const laskin::weekday value, class writer& writer)
  {
    writer.print("static_cast<weekday>(");
    writer.print(std::to_string(static_cast<int>(value)));
    writer.print(")");
  }

  void
  transpile(
    const laskin::value& value,
    class writer& writer,
    const struct options& options
  )
  {
    switch (value.type())
    {
      case laskin::value::type::boolean:
        transpile_boolean(value.as_boolean(), writer);
        break;

      case laskin::value::type::date:
        transpile_date(value.as_date(), writer);
        break;

      case laskin::value::type::month:
        transpile_month(value.as_month(), writer);
        break;

      case laskin::value::type::number:
        transpile_number(value.as_number(), writer);
        break;

      case laskin::value::type::quote:
        transpile_quote(value.as_quote(), writer, options);
        break;

      case laskin::value::type::record:
        transpile_record(value.as_record(), writer, options);
        break;

      case laskin::value::type::string:
        transpile_string(value.as_string(), writer);
        break;

      case laskin::value::type::time:
        transpile_time(value.as_time(), writer);
        break;

      case laskin::value::type::vector:
        transpile_vector(value.as_vector(), writer, options);
        break;

      case laskin::value::type::weekday:
        transpile_weekday(value.as_weekday(), writer);
        break;
    }
  }

  static void
  transpile_record(
    const laskin::node::record_literal::container_type& properties,
    class writer& writer,
    const struct options& options
  )
  {
    writer.println("{");
    writer.indent();
    for (const auto& property : properties)
    {
      writer.print("{ ");
      writer.print(writer::escape(property.first));
      writer.print(", ");
      transpile(property.second, writer, options);
      writer.println(" },");
    }
    writer.dedent();
    writer.print("}");
  }

  static void
  transpile_vector(
    const laskin::node::vector_literal::container_type& elements,
    class writer& writer,
    const struct options& options
  )
  {
    writer.println("{");
    writer.indent();
    for (const auto& element : elements)
    {
      transpile(element, writer, options);
      writer.println(",");
    }
    writer.dedent();
    writer.print("}");
  }

  void
  transpile(
    const std::shared_ptr<laskin::node>& node,
    class writer& writer,
    const struct options& options
  )
  {
    if (!node)
    {
      return;
    }
    switch (node->type())
    {
      case laskin::node::type::definition:
        throw laskin::error(
          laskin::error::type::syntax,
          U"Unable to transpile definition in evaluation context.",
          node->position
        );

      case laskin::node::type::literal:
        transpile(
          std::static_pointer_cast<laskin::node::literal>(node)->value,
          writer,
          options
        );
        break;

      case laskin::node::type::record_literal:
        transpile_record(
          std::static_pointer_cast<laskin::node::record_literal>(
            node
          )->properties,
          writer,
          options
        );
        break;

      case laskin::node::type::symbol:
        {
          const auto id
            = std::static_pointer_cast<laskin::node::symbol>(node)->id;

          if (options.number_optimization && laskin::number::is_valid(id))
          {
            transpile_number(laskin::number::parse(id), writer);
          } else {
            writer.print("c.eval(" + writer::escape(id) + ")");
          }
        }
        break;

      case laskin::node::type::vector_literal:
        transpile_vector(
          std::static_pointer_cast<laskin::node::vector_literal>(
            node
          )->elements,
          writer,
          options
        );
        break;
    }
  }
}
