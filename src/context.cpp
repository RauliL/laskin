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
#include "laskin/context.hpp"
#include "laskin/error.hpp"

namespace laskin
{
  static void initialize_dictionary(
    context::dictionary_type&,
    const context::dictionary_definition&
  );

  namespace api
  {
    extern "C" const context::dictionary_definition boolean;
    extern "C" const context::dictionary_definition date;
    extern "C" const context::dictionary_definition month;
    extern "C" const context::dictionary_definition number;
    extern "C" const context::dictionary_definition quote;
    extern "C" const context::dictionary_definition string;
    extern "C" const context::dictionary_definition utils;
    extern "C" const context::dictionary_definition vector;
    extern "C" const context::dictionary_definition weekday;
  }

  context::context()
  {
    initialize_dictionary(m_dictionary, api::utils);
    initialize_dictionary(m_dictionary, api::boolean);
    initialize_dictionary(m_dictionary, api::date);
    initialize_dictionary(m_dictionary, api::month);
    initialize_dictionary(m_dictionary, api::number);
    initialize_dictionary(m_dictionary, api::quote);
    initialize_dictionary(m_dictionary, api::string);
    initialize_dictionary(m_dictionary, api::vector);
    initialize_dictionary(m_dictionary, api::weekday);
  }

  context::context(const context& that)
    : m_data(that.m_data)
    , m_dictionary(that.m_dictionary) {}

  context& context::operator=(const context& that)
  {
    m_data = that.m_data;
    m_dictionary = that.m_dictionary;

    return *this;
  }

  const value& context::peek() const
  {
    if (!m_data.empty())
    {
      return m_data.back();
    }

    throw error(error::type::range, U"Stack underflow.");
  }

  value context::pop()
  {
    if (!m_data.empty())
    {
      const auto value = m_data.back();

      m_data.pop_back();

      return value;
    }

    throw error(error::type::range, U"Stack underflow.");
  }

  void context::push(const class value& value)
  {
    m_data.push_back(value);
  }

  void context::clear()
  {
    m_data.clear();
  }

  static void initialize_dictionary(context::dictionary_type& dictionary,
                              const context::dictionary_definition& definition)
  {
    for (const auto& word : definition)
    {
      dictionary[word.first] = value::make_quote(quote(word.second));
    }
  }
}
