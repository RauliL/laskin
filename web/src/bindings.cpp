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
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "laskin/ast.hpp"
#include "laskin/chrono.hpp"
#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/quote.hpp"
#include "laskin/value.hpp"

namespace
{
  const char*
  error_type_name(enum laskin::error::type type)
  {
    switch (type)
    {
      case laskin::error::type::syntax:
        return "syntax";
      case laskin::error::type::type:
        return "type";
      case laskin::error::type::unit:
        return "unit";
      case laskin::error::type::range:
        return "range";
      case laskin::error::type::domain:
        return "domain";
      case laskin::error::type::name:
        return "name";
      case laskin::error::type::system:
        return "system";
      case laskin::error::type::exit:
        return "exit";
    }

    return "system";
  }

  std::string
  format_error_message(const laskin::error& error)
  {
    std::ostringstream out;

    out << error;
    return out.str();
  }

  [[noreturn]] void
  raise_laskin_error(const laskin::error& error)
  {
    const auto message = format_error_message(error);
    const auto* type = error_type_name(error.type);
    const int has_position = error.position ? 1 : 0;
    const int line = error.position ? error.position->line : 0;
    const int column = error.position ? error.position->column : 0;

    EM_ASM(
      {
        const err = new Error(UTF8ToString($0));
        err.name = 'LaskinError';
        err.type = UTF8ToString($1);
        if ($2) {
          err.line = $3;
          err.column = $4;
        }
        throw err;
      },
      message.c_str(),
      type,
      has_position,
      line,
      column
    );

    // EM_ASM always throws; this keeps the compiler happy.
    std::abort();
  }

  laskin::value
  value_from_js(const emscripten::val& js_value);

  emscripten::val
  value_to_js(const laskin::value& value);

  std::shared_ptr<laskin::node>
  node_from_js(const emscripten::val& js_node);

  emscripten::val
  node_to_js(const std::shared_ptr<laskin::node>& node);

  void
  set_optional_position(
    emscripten::val& result,
    const std::optional<laskin::position>& position
  )
  {
    if (!position)
    {
      return;
    }

    auto js_position = emscripten::val::object();

    if (position->path)
    {
      js_position.set("path", position->path->string());
    }

    js_position.set("line", position->line);
    js_position.set("column", position->column);
    result.set("position", js_position);
  }

  std::optional<laskin::position>
  position_from_js(const emscripten::val& js_position)
  {
    if (js_position.isUndefined() || js_position.isNull())
    {
      return std::nullopt;
    }

    laskin::position position{
      .path = std::nullopt,
      .line = js_position["line"].as<int>(),
      .column = js_position["column"].as<int>(),
    };
    const auto path = js_position["path"];

    if (!path.isUndefined() && !path.isNull())
    {
      position.path = std::filesystem::path(path.as<std::string>());
    }

    return position;
  }

  emscripten::val
  quote_nodes_to_js(const laskin::quote& quote)
  {
    auto nodes = emscripten::val::array();

    std::visit(
      [&](const auto& alternative)
      {
        using T = std::decay_t<decltype(alternative)>;

        if constexpr (std::is_same_v<T, laskin::native_quote>)
        {
          auto dummy = emscripten::val::object();

          dummy.set(
            "type",
            laskin::node::type_description(laskin::node::type::symbol)
          );
          dummy.set("id", std::u32string(U"native quote"));
          nodes.call<void>("push", dummy);
        } else {
          for (const auto& node : alternative)
          {
            nodes.call<void>("push", node_to_js(node));
          }
        }
      },
      quote
    );

    return nodes;
  }

  emscripten::val
  node_to_js(const std::shared_ptr<laskin::node>& node)
  {
    if (!node)
    {
      return emscripten::val::null();
    }

    auto result = emscripten::val::object();

    result.set("type", laskin::node::type_description(node->type()));
    set_optional_position(result, node->position);

    switch (node->type())
    {
      case laskin::node::type::definition:
        result.set(
          "id",
          std::static_pointer_cast<laskin::node::definition>(node)->id
        );
        break;

      case laskin::node::type::literal:
        result.set(
          "value",
          value_to_js(
            std::static_pointer_cast<laskin::node::literal>(node)->value
          )
        );
        break;

      case laskin::node::type::record_literal:
      {
        auto properties = emscripten::val::object();
        const auto& record =
          std::static_pointer_cast<laskin::node::record_literal>(node)
            ->properties;

        for (const auto& property : record)
        {
          properties.set(property.first, node_to_js(property.second));
        }
        result.set("properties", properties);
        break;
      }

      case laskin::node::type::symbol:
        result.set(
          "id",
          std::static_pointer_cast<laskin::node::symbol>(node)->id
        );
        break;

      case laskin::node::type::vector_literal:
      {
        auto elements = emscripten::val::array();
        const auto& vector =
          std::static_pointer_cast<laskin::node::vector_literal>(node)
            ->elements;

        for (const auto& element : vector)
        {
          elements.call<void>("push", node_to_js(element));
        }
        result.set("elements", elements);
        break;
      }
    }

    return result;
  }

  laskin::scripted_quote
  nodes_from_js(const emscripten::val& js_nodes)
  {
    laskin::scripted_quote nodes;
    const auto length = js_nodes["length"].as<unsigned>();

    nodes.reserve(length);

    for (unsigned i = 0; i < length; ++i)
    {
      nodes.push_back(node_from_js(js_nodes[i]));
    }

    return nodes;
  }

  std::shared_ptr<laskin::node>
  node_from_js(const emscripten::val& js_node)
  {
    const auto type = js_node["type"].as<std::u32string>();
    const auto position = position_from_js(js_node["position"]);

    if (type == U"definition")
    {
      return std::make_shared<laskin::node::definition>(
        js_node["id"].as<std::u32string>(),
        position
      );
    }
    else if (type == U"literal")
    {
      return std::make_shared<laskin::node::literal>(
        value_from_js(js_node["value"]),
        position
      );
    }
    else if (type == U"record-literal")
    {
      laskin::node::record_literal::container_type properties;
      const auto js_properties = js_node["properties"];
      const auto keys = emscripten::val::global("Object")
        .call<emscripten::val>("keys", js_properties);
      const auto key_count = keys["length"].as<unsigned>();

      for (unsigned i = 0; i < key_count; ++i)
      {
        const auto key = keys[i].as<std::u32string>();

        properties.emplace(key, node_from_js(js_properties[key]));
      }

      return std::make_shared<laskin::node::record_literal>(
        properties,
        position
      );
    }
    else if (type == U"symbol")
    {
      return std::make_shared<laskin::node::symbol>(
        js_node["id"].as<std::u32string>(),
        position
      );
    }
    else if (type == U"vector-literal")
    {
      return std::make_shared<laskin::node::vector_literal>(
        nodes_from_js(js_node["elements"]),
        position
      );
    }

    throw laskin::error(
      laskin::error::type::type,
      U"Unknown Laskin AST node type: " + type
    );
  }

  emscripten::val
  value_to_js(const laskin::value& value)
  {
    auto result = emscripten::val::object();

    result.set("type", laskin::value::type_description(value.type()));

    switch (value.type())
    {
      case laskin::value::type::boolean:
        result.set("value", value.as_boolean());
        break;

      case laskin::value::type::number:
      {
        const auto& number = value.as_number();

        result.set("value", number.without_measurement_unit().to_u32string());

        if (const auto& unit = number.measurement_unit())
        {
          result.set("unit", unit->symbol);
        }
        break;
      }

      case laskin::value::type::string:
        result.set("value", value.as_string());
        break;

      case laskin::value::type::vector:
      {
        auto elements = emscripten::val::array();

        for (const auto& element : value.as_vector())
        {
          elements.call<void>("push", value_to_js(element));
        }
        result.set("elements", elements);
        break;
      }

      case laskin::value::type::record:
      {
        auto properties = emscripten::val::object();

        for (const auto& property : value.as_record())
        {
          properties.set(property.first, value_to_js(property.second));
        }
        result.set("properties", properties);
        break;
      }

      case laskin::value::type::quote:
      {
        result.set("nodes", quote_nodes_to_js(value.as_quote()));
        break;
      }

      case laskin::value::type::date:
      {
        const auto& date = value.as_date();

        result.set("year", date.year());
        result.set("month", laskin::value::month_description(date.month()));
        result.set("day", date.day());
        break;
      }

      case laskin::value::type::time:
      {
        const auto& time = value.as_time();

        result.set("hour", time.hour());
        result.set("minute", time.minute());
        result.set("second", time.second());
        break;
      }

      case laskin::value::type::month:
        result.set(
          "value",
          laskin::value::month_description(value.as_month())
        );
        break;

      case laskin::value::type::weekday:
        result.set(
          "value",
          laskin::value::weekday_description(value.as_weekday())
        );
        break;
    }

    return result;
  }

  laskin::vector
  vector_from_js(const emscripten::val& js_elements)
  {
    laskin::vector elements;
    const auto length = js_elements["length"].as<unsigned>();

    elements.reserve(length);

    for (unsigned i = 0; i < length; ++i)
    {
      elements.push_back(value_from_js(js_elements[i]));
    }

    return elements;
  }

  laskin::record
  record_from_js(const emscripten::val& js_properties)
  {
    laskin::record properties;
    const auto keys = emscripten::val::global("Object")
      .call<emscripten::val>("keys", js_properties);
    const auto key_count = keys["length"].as<unsigned>();

    for (unsigned i = 0; i < key_count; ++i)
    {
      const auto key = keys[i].as<std::u32string>();

      properties.emplace(key, value_from_js(js_properties[key]));
    }

    return properties;
  }

  laskin::value
  value_from_js(const emscripten::val& js_value)
  {
    const auto type = js_value["type"].as<std::u32string>();

    if (type == U"boolean")
    {
      return laskin::value(js_value["value"].as<bool>());
    }
    else if (type == U"number")
    {
      auto input = js_value["value"].as<std::u32string>();
      const auto unit = js_value["unit"];

      if (!unit.isUndefined() && !unit.isNull())
      {
        input.append(unit.as<std::u32string>());
      }

      return laskin::value::parse_number(input);
    }
    else if (type == U"string")
    {
      return laskin::value(js_value["value"].as<std::u32string>());
    }
    else if (type == U"vector")
    {
      return laskin::value(vector_from_js(js_value["elements"]));
    }
    else if (type == U"record")
    {
      return laskin::value(record_from_js(js_value["properties"]));
    }
    else if (type == U"quote")
    {
      const auto js_nodes = js_value["nodes"];

      if (!js_nodes.isUndefined() && !js_nodes.isNull())
      {
        return laskin::value(nodes_from_js(js_nodes));
      }

      return laskin::value(
        laskin::parse(js_value["source"].as<std::u32string>())
      );
    }
    else if (type == U"date")
    {
      return laskin::value(laskin::date(
        js_value["year"].as<int>(),
        laskin::parse_month(js_value["month"].as<std::u32string>()),
        js_value["day"].as<int>()
      ));
    }
    else if (type == U"time")
    {
      return laskin::value(laskin::time(
        js_value["hour"].as<int>(),
        js_value["minute"].as<int>(),
        js_value["second"].as<int>()
      ));
    }
    else if (type == U"month")
    {
      return laskin::value(
        laskin::parse_month(js_value["value"].as<std::u32string>())
      );
    }
    else if (type == U"weekday")
    {
      return laskin::value(
        laskin::parse_weekday(js_value["value"].as<std::u32string>())
      );
    }

    throw laskin::error(
      laskin::error::type::type,
      U"Unknown Laskin value type: " + type
    );
  }
}

std::u32string
laskinValueToString(const emscripten::val& js_value)
{
  try
  {
    return value_from_js(js_value).to_string();
  }
  catch (const laskin::error& error)
  {
    raise_laskin_error(error);
  }
}

std::u32string
laskinValueToSource(const emscripten::val& js_value)
{
  try
  {
    return value_from_js(js_value).to_source();
  }
  catch (const laskin::error& error)
  {
    raise_laskin_error(error);
  }
}

/**
 * JavaScript-facing interpreter context. File includes are disabled.
 */
class Context
{
public:
  Context()
    : m_context(nullptr, false) {}

  std::string run(const std::string& source)
  {
    std::stringstream output;

    try
    {
      m_context.run(source, &output);
    }
    catch (const laskin::error& error)
    {
      raise_laskin_error(error);
    }

    return output.str();
  }

  void clear()
  {
    m_context.clear();
  }

  int depth() const
  {
    return static_cast<int>(m_context.data.size());
  }

  emscripten::val peek() const
  {
    try
    {
      return value_to_js(m_context.peek());
    }
    catch (const laskin::error& error)
    {
      raise_laskin_error(error);
    }
  }

  emscripten::val pop()
  {
    try
    {
      return value_to_js(m_context.pop());
    }
    catch (const laskin::error& error)
    {
      raise_laskin_error(error);
    }
  }

  void push(const emscripten::val& js_value)
  {
    try
    {
      m_context.push(value_from_js(js_value));
    }
    catch (const laskin::error& error)
    {
      raise_laskin_error(error);
    }
  }

  emscripten::val stack() const
  {
    auto result = emscripten::val::array();
    const auto& data = m_context.data;

    for (std::size_t i = 0; i < data.size(); ++i)
    {
      // Index 0 is the top of the stack, matching peek()/pop().
      const auto& value = data[data.size() - i - 1];

      result.call<void>("push", value_to_js(value));
    }

    return result;
  }

  emscripten::val dictionary() const
  {
    auto result = emscripten::val::object();
    const auto& dictionary = m_context.dictionary;

    for (const auto& entry : dictionary)
    {
      result.set(entry.first, value_to_js(entry.second));
    }

    return result;
  }

private:
  laskin::context m_context;
};

EMSCRIPTEN_BINDINGS(laskin)
{
  emscripten::class_<Context>("Context")
    .constructor<>()
    .function("run", &Context::run)
    .function("clear", &Context::clear)
    .function("depth", &Context::depth)
    .function("peek", &Context::peek)
    .function("pop", &Context::pop)
    .function("push", &Context::push)
    .function("stack", &Context::stack)
    .function("dictionary", &Context::dictionary);

  emscripten::function("laskinValueToString", &laskinValueToString);
  emscripten::function("laskinValueToSource", &laskinValueToSource);
}
