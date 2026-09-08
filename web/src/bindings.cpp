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

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <peelo/unicode/encoding/utf8.hpp>

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

  std::string
  value_to_utf8(const laskin::value& value)
  {
    return peelo::unicode::encoding::utf8::encode(value.to_string());
  }

  std::string
  value_to_utf8_source(const laskin::value& value)
  {
    return peelo::unicode::encoding::utf8::encode(value.to_source());
  }

  const char*
  value_type_name(enum laskin::value::type type)
  {
    switch (type)
    {
      case laskin::value::type::boolean:
        return "boolean";
      case laskin::value::type::number:
        return "number";
      case laskin::value::type::string:
        return "string";
      case laskin::value::type::vector:
        return "vector";
      case laskin::value::type::record:
        return "record";
      case laskin::value::type::quote:
        return "quote";
      case laskin::value::type::date:
        return "date";
      case laskin::value::type::time:
        return "time";
      case laskin::value::type::month:
        return "month";
      case laskin::value::type::weekday:
        return "weekday";
    }

    return "unknown";
  }

  emscripten::val
  value_to_js(const laskin::value& value)
  {
    using peelo::unicode::encoding::utf8::encode;

    auto result = emscripten::val::object();

    result.set("type", value_type_name(value.type()));

    switch (value.type())
    {
      case laskin::value::type::boolean:
        result.set("value", value.as_boolean());
        break;

      case laskin::value::type::number:
        result.set("value", value_to_utf8(value));
        break;

      case laskin::value::type::string:
        result.set("value", encode(value.as_string()));
        break;

      case laskin::value::type::vector:
      {
        auto elements = emscripten::val::array();

        for (const auto& element : value.as_vector())
        {
          elements.call<void>("push", value_to_js(element));
        }
        result.set("value", elements);
        break;
      }

      case laskin::value::type::record:
      {
        auto properties = emscripten::val::object();

        for (const auto& property : value.as_record())
        {
          properties.set(
            encode(property.first),
            value_to_js(property.second)
          );
        }
        result.set("value", properties);
        break;
      }

      case laskin::value::type::quote:
        result.set("value", encode(value.as_quote().to_source()));
        break;

      case laskin::value::type::date:
      case laskin::value::type::time:
      case laskin::value::type::month:
      case laskin::value::type::weekday:
        result.set("value", value_to_utf8(value));
        break;
    }

    return result;
  }

  laskin::value
  value_from_js(const emscripten::val& js_value)
  {
    using peelo::unicode::encoding::utf8::decode;

    const auto type = js_value["type"].as<std::string>();
    const auto payload = js_value["value"];

    if (type == "boolean")
    {
      return laskin::value(payload.as<bool>());
    }

    if (type == "number")
    {
      return laskin::value::parse_number(decode(payload.as<std::string>()));
    }

    if (type == "string")
    {
      return laskin::value(decode(payload.as<std::string>()));
    }

    if (type == "vector")
    {
      laskin::vector elements;
      const auto length = payload["length"].as<unsigned>();

      elements.reserve(length);

      for (unsigned i = 0; i < length; ++i)
      {
        elements.push_back(value_from_js(payload[i]));
      }

      return laskin::value(elements);
    }

    if (type == "record")
    {
      laskin::record properties;
      const auto keys = emscripten::val::global("Object")
        .call<emscripten::val>("keys", payload);
      const auto key_count = keys["length"].as<unsigned>();

      for (unsigned i = 0; i < key_count; ++i)
      {
        const auto key = keys[i].as<std::string>();

        properties.emplace(decode(key), value_from_js(payload[key]));
      }

      return laskin::value(properties);
    }

    if (type == "quote")
    {
      return laskin::value(laskin::quote::parse(payload.as<std::string>()));
    }

    if (type == "date")
    {
      return laskin::value(laskin::parse_date(decode(payload.as<std::string>())));
    }

    if (type == "time")
    {
      return laskin::value(laskin::parse_time(decode(payload.as<std::string>())));
    }

    if (type == "month")
    {
      return laskin::value(laskin::parse_month(decode(payload.as<std::string>())));
    }

    if (type == "weekday")
    {
      return laskin::value(laskin::parse_weekday(decode(payload.as<std::string>())));
    }

    throw laskin::error(
      laskin::error::type::type,
      "Unknown Laskin value type: " + type
    );
  }
}

std::string
laskinValueToString(const emscripten::val& js_value)
{
  try
  {
    return value_to_utf8(value_from_js(js_value));
  }
  catch (const laskin::error& error)
  {
    raise_laskin_error(error);
  }
}

std::string
laskinValueToSource(const emscripten::val& js_value)
{
  try
  {
    return value_to_utf8_source(value_from_js(js_value));
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
    using peelo::unicode::encoding::utf8::encode;

    auto result = emscripten::val::object();
    const auto& dictionary = m_context.dictionary;

    for (const auto& entry : dictionary)
    {
      result.set(encode(entry.first), value_to_js(entry.second));
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
