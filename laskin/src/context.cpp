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

#include "laskin/chrono.hpp"
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

  void
  context::lookup(
    const std::u32string& id,
    std::ostream* out,
    const std::optional<struct position>& position
  )
  {
    if (!data.empty())
    {
      const auto& value = data.back();
      const auto type_id = value::type_description(value.type()) + U':' + id;
      const auto word = dictionary.find(type_id);

      if (word != std::end(dictionary))
      {
        if (word->second.is(value::type::quote))
        {
          word->second.as_quote().call(*this, out);
        } else {
          data.push_back(word->second);
        }
        return;
      }
    }

    {
      const auto word = dictionary.find(id);

      if (word != std::end(dictionary))
      {
        if (word->second.is(value::type::quote))
        {
          word->second.as_quote().call(*this, out);
        } else {
          data.push_back(word->second);
        }
        return;
      }
    }

    if (number::is_valid(id))
    {
      data.push_back(value::parse_number(id));
      return;
    }

    if (is_date(id))
    {
      data.push_back(parse_date(id));
      return;
    }

    if (is_time(id))
    {
      data.push_back(parse_time(id));
      return;
    }

    if (default_callback)
    {
      if (const auto value = default_callback(id))
      {
        data.push_back(*value);
        return;
      }
    }

    throw error(
      error::type::name,
      U"Unrecognized symbol: `" + id + U"'",
      position
    );
  }

  value
  context::eval(
    const std::u32string& id,
    const std::optional<struct position>& position
  )
  {
    if (id == U"true")
    {
      return true;
    }

    if (id == U"false")
    {
      return false;
    }

    if (id == U"drop")
    {
      return pop();
    }

    if (number::is_valid(id))
    {
      return value::parse_number(id);
    }

    if (is_date(id))
    {
      return parse_date(id);
    }

    if (is_time(id))
    {
      return parse_time(id);
    }

    if (is_month(id))
    {
      return parse_month(id);
    }

    if (is_weekday(id))
    {
      return parse_weekday(id);
    }

    if (default_callback)
    {
      if (const auto value = default_callback(id))
      {
        return *value;
      }
    }

    throw error(
      error::type::name,
      U"Unable to evaluate `" + id + U"' as expression.",
      position
    );
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

  context&
  context::operator>>(std::string& value)
  {
    using peelo::unicode::encoding::utf8::encode;

    value = encode(pop().as_string());

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
      dictionary[word.first] = word.second;
    }
  }
}
