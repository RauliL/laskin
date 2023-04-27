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

    explicit context();
    context(const context& that);
    context& operator=(const context& that);

    /**
     * Returns data stack of the context.
     */
    inline container_type& data()
    {
      return m_data;
    }

    /**
     * Returns data stack of the context.
     */
    inline const container_type& data() const
    {
      return m_data;
    }

    /**
     * Returns dictionary of the context.
     */
    inline dictionary_type& dictionary()
    {
      return m_dictionary;
    }

    /**
     * Returns dictionary of the context.
     */
    inline const dictionary_type& dictionary() const
    {
      return m_dictionary;
    }

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
    inline context& operator<<(const class value& value)
    {
      push(value);

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

  private:
    /** Container for stack data. */
    container_type m_data;
    /** Container for definitions. */
    dictionary_type m_dictionary;
  };
}
