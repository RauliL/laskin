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

    context(const context&) = default;
    context(context&&) = default;
    context& operator=(const context& that) = default;
    context& operator=(context&&) = default;

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
    void push(const class value& value);

    /**
     * Clears the entire data stack.
     */
    void clear();

    /**
     * Pushes given value onto the stack.
     */
    context& operator<<(const class value& value);

    /**
     * Pushes given boolean value onto the stack.
     */
    context& operator<<(bool value);

    /**
     * Pushes given numeric value onto the stack.
     */
    context& operator<<(const number& value);

    /**
     * Pushes given numeric value onto the stack.
     */
    context& operator<<(double value);

    /**
     * Pushes given numeric value onto the stack.
     */
    context& operator<<(int value);

    /**
     * Pushes given string onto the stack.
     */
    context& operator<<(const std::u32string& value);

    /**
     * Pushes given quote onto the stack.
     */
    context& operator<<(const quote& value);

    /**
     * Pushes given vector onto the stack.
     */
    context& operator<<(const value::vector_container& elements);

    /**
     * Pushes given record onto the stack.
     */
    context& operator<<(const value::record_container& properties);

    /**
     * Pops value from the stack and places it into given slot.
     */
    context& operator>>(class value& value);
  };
}
