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
#pragma once

#include <deque>
#include <unordered_map>

#include "laskin/quote.hpp"

namespace laskin
{
  class context
  {
  public:
    using container_type = std::deque<value>;
    using dictionary_type = std::unordered_map<std::u32string, value>;
    using dictionary_definition = std::initializer_list<
      std::pair<std::u32string, quote::callback>
    >;
    using dictionary_default_callback = std::function<
      std::optional<value>(const std::u32string&)
    >;

    /** Container for stack data. */
    container_type data;
    /** Container for dictionary definitions. */
    dictionary_type dictionary;
    /** Invoked when dictionary item is missing. */
    dictionary_default_callback default_callback;
    /** Whether include word should be allowed or not. */
    bool allow_include;

    explicit context(
      const dictionary_default_callback& default_callback_ = nullptr,
      bool allow_include_ = true
    );

    LASKIN_DEFAULT_COPY_AND_ASSIGN(context);

    /**
     * Evaluates given source code as program.
     */
    inline void run(
      const std::u32string& source,
      std::ostream* out = nullptr,
      const std::optional<std::filesystem::path>& path = std::nullopt,
      int line = 1,
      int column = 0
    )
    {
      quote::parse(source, path, line, column).call(*this, out);
    }

    /**
     * Evaluates given source code as program. The input is expected to encoded
     * with UTF-8 character encoding.
     */
    inline void run(
      const std::string& source,
      std::ostream* out = nullptr,
      const std::optional<std::filesystem::path>& path = std::nullopt,
      int line = 1,
      int column = 0
    )
    {
      quote::parse(source, path, line, column).call(*this, out);
    }

    /**
     * Evaluates source code read from given input stream as a program. The
     * input is expected to be encoded with UTF-8 character encoding.
     */
    inline void run(
      std::istream& input,
      std::ostream* out = nullptr,
      const std::optional<std::filesystem::path>& path = std::nullopt,
      int line = 1,
      int column = 0
    )
    {
      quote::parse(input, path, line, column).call(*this, out);
    }

    /**
     * Includes and executes given program file.
     */
    void include(
      const std::filesystem::path& path,
      std::ostream* out = nullptr
    );

    /**
     * Performs an dictionary lookup on the context or throws `error` instance
     * if the given identifier/symbol cannot be found from the dictionary or
     * does not contain number, date or time data.
     */
    void lookup(
      const std::u32string& id,
      std::ostream* out = nullptr,
      const std::optional<struct position>& position = std::nullopt
    );

    /**
     * Evaluates given identifier/symbol as expression. No dictionary lookup
     * will be done but certain constants such as `true`, `false` and such are
     * converted into values as well as symbols containing number, date or time
     * data.
     */
    value eval(
      const std::u32string& id,
      const std::optional<struct position>& position = std::nullopt
   );

    /**
     * Returns the topmost value from the stack. If the stack is empty, range
     * error will be thrown.
     */
    const value& peek() const;

    /**
     * Pops value from top of the stack and returns it. If the stack is empty,
     * range error will be thrown.
     */
    value pop();

    /**
     * Pushes given value onto the stack.
     */
    inline void push(const class value& value)
    {
      data.push_back(value);
    }

    /**
     * Clears the entire data stack.
     */
    inline void clear()
    {
      data.clear();
    }

    /**
     * Pushes given value onto the stack.
     */
    inline context& operator<<(const class value& value)
    {
      data.push_back(value);

      return *this;
    }

    /**
     * Pops value from the stack and places it into given slot.
     */
    inline context& operator>>(class value& value)
    {
      value = pop();

      return *this;
    }

    /**
     * Pops boolean value from the stack and places it into given slot.
     */
    inline context& operator>>(bool& value)
    {
      value = pop().as_boolean();

      return *this;
    }

    /**
     * Pops numeric value from the stack and places it into given slot.
     */
    inline context& operator>>(number& value)
    {
      value = pop().as_number();

      return *this;
    }

    /**
     * Pops numeric value from the stack and places it into given slot.
     */
    inline context& operator>>(long& value)
    {
      value = long(pop());

      return *this;
    }

    /**
     * Pops numeric value from the stack and places it into given slot.
     */
    inline context& operator>>(double& value)
    {
      value = double(pop());

      return *this;
    }

    /**
     * Pops string from the stack and places it into given slot.
     */
    inline context& operator>>(std::u32string& value)
    {
      value = pop().as_string();

      return *this;
    }

    /**
     * Pops string from the stack and places it into given slot after encoding
     * it with UTF-8 encoding.
     */
    context& operator>>(std::string& value);

    /**
     * Pops quote from the stack and places it into given slot.
     */
    inline context& operator>>(quote& value)
    {
      value = pop().as_quote();

      return *this;
    }

    /**
     * Pops vector from the stack and places it into given slot.
     */
    inline context& operator>>(vector& elements)
    {
      elements = pop().as_vector();

      return *this;
    }

    /**
     * Pops record from the stack and places it into given slot.
     */
    inline context& operator>>(record& properties)
    {
      properties = pop().as_record();

      return *this;
    }

    /**
     * Returns `false` if the data stack is empty, `true` otherwise.
     */
    inline explicit operator bool() const noexcept
    {
      return !data.empty();
    }

    /**
     * Negates boolean conversion. Returns `true` if the data stack is empty,
     * `false` otherwise.
     */
    inline bool operator!() const noexcept
    {
      return data.empty();
    }
  };
}
