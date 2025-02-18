/*
 * Copyright (c) 2023, Rauli Laine
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
  static void w_size(class context& context, std::ostream&)
  {
    const auto& properties = context.peek().as_record();

    context << value::make_number(static_cast<std::int64_t>(properties.size()));
  }

  static void w_keys(class context& context, std::ostream&)
  {
    const auto& properties = context.peek().as_record();
    std::vector<value> result;

    result.reserve(properties.size());
    for (const auto& property : properties)
    {
      result.push_back(value::make_string(property.first));
    }
    context << value::make_vector(result);
  }

  static void w_values(class context& context, std::ostream&)
  {
    const auto& properties = context.peek().as_record();
    std::vector<value> result;

    result.reserve(properties.size());
    for (const auto& property : properties)
    {
      result.push_back(property.second);
    }
    context << value::make_vector(result);
  }

  static void w_for_each(class context& context, std::ostream& out)
  {
    const auto properties = context.pop().as_record();
    const auto quote = context.pop().as_quote();

    for (const auto& property : properties)
    {
      context << value::make_string(property.first) << property.second;
      quote.call(context, out);
    }
  }

  static void w_map(class context& context, std::ostream& out)
  {
    const auto properties = context.pop().as_record();
    const auto quote = context.pop().as_quote();
    tsl::ordered_map<std::u32string, value> new_properties;

    for (const auto& property : properties)
    {
      std::u32string key;
      class value value;

      context << value::make_string(property.first) << property.second;
      quote.call(context, out);
      value = context.pop();
      key = context.pop().as_string();
      new_properties[key] = value;
    }

    context << value::make_record(new_properties);
  }

  static void w_filter(class context& context, std::ostream& out)
  {
    const auto properties = context.pop().as_record();
    const auto quote = context.pop().as_quote();
    tsl::ordered_map<std::u32string, value> new_properties;

    for (const auto& property : properties)
    {
      context << value::make_string(property.first) << property.second;
      quote.call(context, out);
      if (context.pop().as_boolean())
      {
        new_properties[property.first] = property.second;
      }
    }

    context << value::make_record(new_properties);
  }

  static void w_at(class context& context, std::ostream&)
  {
    const auto properties = context.pop().as_record();
    const auto key = context.pop().as_string();
    const auto i = properties.find(key);

    if (i == std::end(properties))
    {
      throw error(error::type::range, U"Record index out of bounds.");
    }
    context << i->second;
  }

  static void w_set(class context& context, std::ostream&)
  {
    auto properties = context.pop().as_record();
    const auto key = context.pop().as_string();
    const auto value = context.pop();

    properties[key] = value;
    context << value::make_record(properties);
  }

  static void w_to_record(class context& context, std::ostream&)
  {
    const auto properties = context.pop().as_record();
    std::vector<value> values;

    values.reserve(properties.size());
    for (const auto& property : properties)
    {
      std::vector<value> pair;

      pair.reserve(2);
      pair.push_back(value::make_string(property.first));
      pair.push_back(property.second);
      values.push_back(value::make_vector(pair));
    }

    context << value::make_vector(values);
  }

  namespace api
  {
    extern "C" const context::dictionary_definition record =
    {
      { U"record:size", w_size },
      { U"record:keys", w_keys },
      { U"record:values", w_values },

      // Iteration.
      { U"record:for-each", w_for_each },
      { U"record:map", w_map },
      { U"record:filter", w_filter },

      // Element access.
      { U"record:@", w_at },
      { U"record:@=", w_set },

      // Conversions.
      { U"record:>vector", w_to_record }
    };
  }
}
