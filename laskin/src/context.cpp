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
#include <fstream>

#include <peelo/unicode/encoding/utf8.hpp>

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
    extern "C" const context::dictionary_definition record;
    extern "C" const context::dictionary_definition string;
    extern "C" const context::dictionary_definition time_api;
    extern "C" const context::dictionary_definition utils;
    extern "C" const context::dictionary_definition vector;
    extern "C" const context::dictionary_definition weekday;
  }

  context::context(
    const dictionary_default_callback& default_callback_,
    bool allow_include_
  )
    : default_callback(default_callback_)
    , allow_include(allow_include_)
  {
    initialize_dictionary(dictionary, api::utils);
    initialize_dictionary(dictionary, api::boolean);
    initialize_dictionary(dictionary, api::date);
    initialize_dictionary(dictionary, api::month);
    initialize_dictionary(dictionary, api::number);
    initialize_dictionary(dictionary, api::quote);
    initialize_dictionary(dictionary, api::record);
    initialize_dictionary(dictionary, api::string);
    initialize_dictionary(dictionary, api::time_api);
    initialize_dictionary(dictionary, api::vector);
    initialize_dictionary(dictionary, api::weekday);
  }

  void
  context::run(
    const std::u32string& source,
    std::ostream* out,
    const std::optional<std::filesystem::path>& path,
    int line,
    int column
  )
  {
    quote::parse(source, path, line, column).call(*this, out);
  }

  void
  context::run(
    const std::string& source,
    std::ostream* out,
    const std::optional<std::filesystem::path>& path,
    int line,
    int column
  )
  {
    using peelo::unicode::encoding::utf8::decode_validate;

    std::u32string decoded_source;

    if (!decode_validate(source, decoded_source))
    {
      throw error(
        error::type::system,
        U"Unable to decode contents of the file with UTF-8 character "
        U"encoding",
        std::make_optional<position>({ path, line, column })
      );
    }
    run(decoded_source, out, path, line, column);
  }

  void
  context::include(const std::filesystem::path& path, std::ostream* out)
  {
    using peelo::unicode::encoding::utf8::decode;
    using peelo::unicode::encoding::utf8::decode_validate;

    if (allow_include)
    {
      std::ifstream in(path);
      std::string source;

      if (!in.good())
      {
        throw error(
          error::type::system,
          U"Unable to open file `" + decode(path) + U"' for reading."
        );
      }
      source.assign(
        std::istreambuf_iterator<char>(in),
        std::istreambuf_iterator<char>()
      );
      in.close();
      run(source, out, path);
    } else {
      throw error(
        error::type::system,
        U"Using include has been disabled in this context."
      );
    }
  }

  const value&
  context::peek() const
  {
    if (!data.empty())
    {
      return data.back();
    }

    throw error(error::type::range, U"Stack underflow.");
  }

  value
  context::pop()
  {
    if (!data.empty())
    {
      const auto value = data.back();

      data.pop_back();

      return value;
    }

    throw error(error::type::range, U"Stack underflow.");
  }

  void
  context::push(const class value& value)
  {
    data.push_back(value);
  }

  void
  context::clear()
  {
    data.clear();
  }

  context&
  context::operator<<(const class value& value)
  {
    push(value);

    return *this;
  }

  context&
  context::operator<<(bool value)
  {
    push(value::make_boolean(value));

    return *this;
  }

  context&
  context::operator<<(const peelo::number& value)
  {
    push(value::make_number(value));

    return *this;
  }

  context&
  context::operator<<(double value)
  {
    push(value::make_number(value));

    return *this;
  }

  context&
  context::operator<<(int value)
  {
    push(value::make_number(value));

    return *this;
  }

  context&
  context::operator<<(const std::u32string& value)
  {
    push(value::make_string(value));

    return *this;
  }

  context&
  context::operator<<(const std::string& value)
  {
    using peelo::unicode::encoding::utf8::decode;

    push(value::make_string(decode(value)));

    return *this;
  }

  context&
  context::operator<<(const quote& value)
  {
    push(value::make_quote(value));

    return *this;
  }

  context&
  context::operator<<(const value::vector_container& elements)
  {
    push(value::make_vector(elements));

    return *this;
  }

  context&
  context::operator<<(const value::record_container& properties)
  {
    push(value::make_record(properties));

    return *this;
  }

  context&
  context::operator>>(class value& value)
  {
    value = pop();

    return *this;
  }

  context&
  context::operator>>(bool& value)
  {
    value = pop().as_boolean();

    return *this;
  }

  context&
  context::operator>>(peelo::number& value)
  {
    value = pop().as_number();

    return *this;
  }

  context&
  context::operator>>(double& value)
  {
    try
    {
      value = double(pop().as_number());
    }
    catch (const std::underflow_error&)
    {
      throw error(error::type::range, U"Numeric value too small.");
    }
    catch (const std::overflow_error&)
    {
      throw error(error::type::range, U"Numeric value too large.");
    }

    return *this;
  }

  context&
  context::operator>>(std::u32string& value)
  {
    value = pop().as_string();

    return *this;
  }

  context&
  context::operator>>(std::string& value)
  {
    using peelo::unicode::encoding::utf8::encode;

    value = encode(pop().as_string());

    return *this;
  }

  context&
  context::operator>>(quote& value)
  {
    value = pop().as_quote();

    return *this;
  }

  context&
  context::operator>>(value::vector_container& elements)
  {
    elements = pop().as_vector();

    return *this;
  }

  context&
  context::operator>>(value::record_container& properties)
  {
    properties = pop().as_record();

    return *this;
  }

  static void
  initialize_dictionary(
    context::dictionary_type& dictionary,
    const context::dictionary_definition& definition
  )
  {
    for (const auto& word : definition)
    {
      dictionary[word.first] = value::make_quote(quote(word.second));
    }
  }
}
