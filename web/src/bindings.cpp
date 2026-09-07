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

#include "laskin/context.hpp"
#include "laskin/error.hpp"

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
    const int line = error.position ? error.position->line : 0;
    const int column = error.position ? error.position->column : 0;

    EM_ASM(
      {
        const err = new Error(UTF8ToString($0));
        err.name = 'LaskinError';
        err.type = UTF8ToString($1);
        if ($2 > 0) {
          err.line = $2;
        }
        if ($3 > 0) {
          err.column = $3;
        }
        throw err;
      },
      message.c_str(),
      type,
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

  std::string peek() const
  {
    try
    {
      return value_to_utf8(m_context.peek());
    }
    catch (const laskin::error& error)
    {
      raise_laskin_error(error);
    }
  }

  std::string pop()
  {
    try
    {
      return value_to_utf8(m_context.pop());
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
      result.call<void>("push", value_to_utf8(value));
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
    .function("stack", &Context::stack);
}
