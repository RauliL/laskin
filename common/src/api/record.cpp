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
  static void w_size(class context& context, std::ostream& out)
  {
    const auto& properties = context.peek().as_record();

    context << value::make_number(static_cast<std::int64_t>(properties.size()));
  }

  static void w_keys(class context& context, std::ostream& out)
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

  static void w_values(class context& context, std::ostream& out)
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

  static void w_at(class context& context, std::ostream& out)
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

  namespace api
  {
    extern "C" const context::dictionary_definition record =
    {
      { U"record:size", w_size },
      { U"record:keys", w_keys },
      { U"record:values", w_values },
      { U"record:@", w_at }
    };
  }
}
