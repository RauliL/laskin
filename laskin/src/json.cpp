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
#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/chrono.hpp"
#include "laskin/error.hpp"
#include "laskin/json.hpp"
#include "laskin/quote.hpp"

namespace laskin
{
  using peelo::unicode::encoding::utf8::decode;
  using peelo::unicode::encoding::utf8::encode;

  static inline std::string
  to_utf8(const std::u32string& input)
  {
    return encode(input);
  }

  static inline std::u32string
  from_utf8(const std::string& input)
  {
    return decode(input);
  }

  static std::u32string
  require_string(const nlohmann::json& json, const char* key)
  {
    const auto& value = json.at(key);

    if (!value.is_string())
    {
      throw error(
        error::type::type,
        std::string("Expected string property \"") + key + "\""
      );
    }

    return from_utf8(value.get_ref<const std::string&>());
  }

  static void
  set_optional_position(
    nlohmann::json& json,
    const std::optional<position>& position
  )
  {
    if (!position)
    {
      return;
    }

    auto position_json = nlohmann::json::object();

    if (position->path)
    {
      position_json["path"] = position->path->string();
    }

    position_json["line"] = position->line;
    position_json["column"] = position->column;
    json["position"] = std::move(position_json);
  }

  static std::optional<position>
  position_from_json(const nlohmann::json& json)
  {
    if (!json.contains("position") || json["position"].is_null())
    {
      return std::nullopt;
    }

    const auto& position_json = json.at("position");
    position result{
      .path = std::nullopt,
      .line = position_json.at("line").get<int>(),
      .column = position_json.at("column").get<int>(),
    };

    if (position_json.contains("path") && !position_json["path"].is_null())
    {
      result.path = std::filesystem::path(
        position_json.at("path").get<std::string>()
      );
    }

    return result;
  }

  static nlohmann::json
  quote_nodes_to_json(const quote& quote)
  {
    auto nodes = nlohmann::json::array();

    std::visit(
      [&](const auto& alternative)
      {
        using T = std::decay_t<decltype(alternative)>;

        if constexpr (std::is_same_v<T, native_quote>)
        {
          nodes.push_back({
            {"type", "symbol"},
            {"id", "native quote"},
          });
        } else {
          for (const auto& node : alternative)
          {
            nodes.push_back(node);
          }
        }
      },
      quote
    );

    return nodes;
  }

  static scripted_quote
  nodes_from_json(const nlohmann::json& json)
  {
    scripted_quote nodes;

    if (!json.is_array())
    {
      throw error(error::type::type, U"Expected AST node array");
    }

    nodes.reserve(json.size());

    for (const auto& element : json)
    {
      nodes.push_back(element.get<std::shared_ptr<node>>());
    }

    return nodes;
  }

  /**
   * Tests whether the given value is a native (C++) quote.
   */
  static inline bool
  is_native_quote(const value& value)
  {
    return value.is(value::type::quote)
      && std::holds_alternative<native_quote>(value.as_quote());
  }

  void
  to_json(nlohmann::json& json, const value& value)
  {
    json = nlohmann::json::object();
    json["type"] = to_utf8(value::type_description(value.type()));

    switch (value.type())
    {
      case value::type::boolean:
        json["value"] = value.as_boolean();
        break;

      case value::type::number:
      {
        const auto& number = value.as_number();

        json["value"] = to_utf8(
          number.without_measurement_unit().to_u32string()
        );

        if (const auto& unit = number.measurement_unit())
        {
          json["unit"] = unit->symbol;
        }
        break;
      }

      case value::type::string:
        json["value"] = to_utf8(value.as_string());
        break;

      case value::type::vector:
      {
        auto elements = nlohmann::json::array();

        for (const auto& element : value.as_vector())
        {
          elements.push_back(element);
        }
        json["elements"] = std::move(elements);
        break;
      }

      case value::type::record:
      {
        auto properties = nlohmann::json::object();

        for (const auto& property : value.as_record())
        {
          properties[to_utf8(property.first)] = property.second;
        }
        json["properties"] = std::move(properties);
        break;
      }

      case value::type::quote:
        json["nodes"] = quote_nodes_to_json(value.as_quote());
        break;

      case value::type::date:
      {
        const auto& date = value.as_date();

        json["year"] = date.year();
        json["month"] = to_utf8(value::month_description(date.month()));
        json["day"] = date.day();
        break;
      }

      case value::type::time:
      {
        const auto& time = value.as_time();

        json["hour"] = time.hour();
        json["minute"] = time.minute();
        json["second"] = time.second();
        break;
      }

      case value::type::month:
        json["value"] = to_utf8(value::month_description(value.as_month()));
        break;

      case value::type::weekday:
        json["value"] = to_utf8(
          value::weekday_description(value.as_weekday())
        );
        break;
    }
  }

  void
  from_json(const nlohmann::json& json, value& result)
  {
    if (!json.is_object())
    {
      throw error(error::type::type, U"Expected Laskin value object");
    }

    const auto type = require_string(json, "type");

    if (type == U"boolean")
    {
      result = value(json.at("value").get<bool>());
    }
    else if (type == U"number")
    {
      auto input = require_string(json, "value");

      if (json.contains("unit") && !json["unit"].is_null())
      {
        input.append(require_string(json, "unit"));
      }

      result = value::parse_number(input);
    }
    else if (type == U"string")
    {
      result = value(require_string(json, "value"));
    }
    else if (type == U"vector")
    {
      vector elements;

      for (const auto& element : json.at("elements"))
      {
        elements.push_back(element.get<value>());
      }
      result = value(elements);
    }
    else if (type == U"record")
    {
      record properties;

      for (const auto& property : json.at("properties").items())
      {
        properties.emplace(
          from_utf8(property.key()),
          property.value().get<value>()
        );
      }
      result = value(properties);
    }
    else if (type == U"quote")
    {
      if (json.contains("nodes") && !json["nodes"].is_null())
      {
        result = value(nodes_from_json(json.at("nodes")));
      } else {
        result = value(parse(require_string(json, "source")));
      }
    }
    else if (type == U"date")
    {
      result = value(date(
        json.at("year").get<int>(),
        parse_month(require_string(json, "month")),
        json.at("day").get<int>()
      ));
    }
    else if (type == U"time")
    {
      result = value(time(
        json.at("hour").get<int>(),
        json.at("minute").get<int>(),
        json.at("second").get<int>()
      ));
    }
    else if (type == U"month")
    {
      result = value(parse_month(require_string(json, "value")));
    }
    else if (type == U"weekday")
    {
      result = value(parse_weekday(require_string(json, "value")));
    } else {
      throw error(
        error::type::type,
        U"Unknown Laskin value type: " + type
      );
    }
  }

  void
  to_json(nlohmann::json& json, const std::shared_ptr<node>& node)
  {
    if (!node)
    {
      json = nullptr;
      return;
    }

    json = nlohmann::json::object();
    json["type"] = to_utf8(node::type_description(node->type()));
    set_optional_position(json, node->position);

    switch (node->type())
    {
      case node::type::definition:
        json["id"] = to_utf8(
          std::static_pointer_cast<node::definition>(node)->id
        );
        break;

      case node::type::literal:
        json["value"] = std::static_pointer_cast<node::literal>(node)->value;
        break;

      case node::type::record:
      {
        auto properties = nlohmann::json::object();
        const auto& record =
          std::static_pointer_cast<node::record>(node)->properties;

        for (const auto& property : record)
        {
          properties[to_utf8(property.first)] = property.second;
        }
        json["properties"] = std::move(properties);
        break;
      }

      case node::type::symbol:
        json["id"] = to_utf8(
          std::static_pointer_cast<node::symbol>(node)->id
        );
        break;

      case node::type::vector:
      {
        auto elements = nlohmann::json::array();
        const auto& vector =
          std::static_pointer_cast<node::vector>(node)->elements;

        for (const auto& element : vector)
        {
          elements.push_back(element);
        }
        json["elements"] = std::move(elements);
        break;
      }
    }
  }

  void
  from_json(const nlohmann::json& json, std::shared_ptr<node>& result)
  {
    if (json.is_null())
    {
      result = nullptr;
      return;
    }

    if (!json.is_object())
    {
      throw error(error::type::type, U"Expected Laskin AST node object");
    }

    const auto type = require_string(json, "type");
    const auto position = position_from_json(json);

    if (type == U"definition")
    {
      result = std::make_shared<node::definition>(
        require_string(json, "id"),
        position
      );
    }
    else if (type == U"literal")
    {
      result = std::make_shared<node::literal>(
        json.at("value").get<value>(),
        position
      );
    }
    else if (type == U"record")
    {
      node::record::container_type properties;

      for (const auto& property : json.at("properties").items())
      {
        properties.emplace(
          from_utf8(property.key()),
          property.value().get<std::shared_ptr<node>>()
        );
      }
      result = std::make_shared<node::record>(properties, position);
    }
    else if (type == U"symbol")
    {
      result = std::make_shared<node::symbol>(
        require_string(json, "id"),
        position
      );
    }
    else if (type == U"vector")
    {
      result = std::make_shared<node::vector>(
        nodes_from_json(json.at("elements")),
        position
      );
    } else {
      throw error(
        error::type::type,
        U"Unknown Laskin AST node type: " + type
      );
    }
  }

  void
  to_json(nlohmann::json& json, const context& context)
  {
    auto stack = nlohmann::json::array();
    auto dictionary = nlohmann::json::object();
    const auto& data = context.data;

    for (std::size_t i = 0; i < data.size(); ++i)
    {
      // Index 0 is the top of the stack.
      stack.push_back(data[data.size() - i - 1]);
    }

    for (const auto& entry : context.dictionary)
    {
      if (is_native_quote(entry.second))
      {
        continue;
      }

      dictionary[to_utf8(entry.first)] = entry.second;
    }

    json = {
      {"stack", std::move(stack)},
      {"dictionary", std::move(dictionary)},
    };
  }

  void
  from_json(const nlohmann::json& json, context& context)
  {
    if (!json.is_object())
    {
      throw error(error::type::type, U"Expected Laskin context object");
    }

    context.data.clear();

    {
      const auto& stack = json.at("stack");

      if (!stack.is_array())
      {
        throw error(error::type::type, U"Expected context stack array");
      }

      // JSON index 0 is top of stack; push bottom-first.
      for (auto it = stack.rbegin(); it != stack.rend(); ++it)
      {
        context.push(it->get<value>());
      }
    }

    for (auto it = context.dictionary.begin();
         it != context.dictionary.end();)
    {
      if (is_native_quote(it->second))
      {
        ++it;
      } else {
        it = context.dictionary.erase(it);
      }
    }

    {
      const auto& dictionary = json.at("dictionary");

      if (!dictionary.is_object())
      {
        throw error(error::type::type, U"Expected context dictionary object");
      }

      for (const auto& entry : dictionary.items())
      {
        context.dictionary[from_utf8(entry.key())] = entry.value().get<value>();
      }
    }
  }

  void
  load_snapshot(context& context, const std::filesystem::path& path)
  {
    if (!context.allow_snapshot)
    {
      throw error(
        error::type::system,
        U"Using snapshot has been disabled in this context."
      );
    }

    std::ifstream input(path);

    if (!input.good())
    {
      throw error(
        error::type::system,
        U"Unable to open snapshot file `" +
          peelo::unicode::encoding::utf8::decode(path.string()) +
          U"' for reading."
      );
    }

    load_snapshot(context, input);
  }

  void
  load_snapshot(context& context, std::istream& input)
  {
    if (!context.allow_snapshot)
    {
      throw error(
        error::type::system,
        U"Using snapshot has been disabled in this context."
      );
    }

    try
    {
      const auto json = nlohmann::json::parse(input);

      from_json(json, context);
    }
    catch (const nlohmann::json::exception& e)
    {
      throw error(error::type::syntax, e.what());
    }
  }

  void
  save_snapshot(const context& context, const std::filesystem::path& path)
  {
    if (!context.allow_snapshot)
    {
      throw error(
        error::type::system,
        U"Using snapshot has been disabled in this context."
      );
    }

    std::ofstream output(path);

    if (!output.good())
    {
      throw error(
        error::type::system,
        U"Unable to open snapshot file `" +
          peelo::unicode::encoding::utf8::decode(path.string()) +
          U"' for writing."
      );
    }

    save_snapshot(context, output);
  }

  void
  save_snapshot(const context& context, std::ostream& output)
  {
    if (!context.allow_snapshot)
    {
      throw error(
        error::type::system,
        U"Using snapshot has been disabled in this context."
      );
    }

    nlohmann::json json;

    to_json(json, context);
    output << json.dump(2) << '\n';

    if (!output.good())
    {
      throw error(
        error::type::system,
        U"Unable to write context snapshot."
      );
    }
  }
}
