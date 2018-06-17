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
  static void w_max(class context& context, std::ostream&)
  {
    const auto vec = context.pop().as_vector();
    const auto size = vec.size();

    if (size > 0)
    {
      auto largest = vec[0];

      for (std::vector<value>::size_type i = 1; i < size; ++i)
      {
        const auto& candidate = vec[i];

        if (candidate > largest)
        {
          largest = candidate;
        }
      }
      context << largest;
      return;
    }

    throw error(error::type_range, U"Vector is empty.");
  }

  static void w_min(class context& context, std::ostream&)
  {
    const auto vec = context.pop().as_vector();
    const auto size = vec.size();

    if (size > 0)
    {
      auto smallest = vec[0];

      for (std::vector<value>::size_type i = 1; i < size; ++i)
      {
        const auto& candidate = vec[i];

        if (candidate < smallest)
        {
          smallest = candidate;
        }
      }
      context << smallest;
      return;
    }

    throw error(error::type_range, U"Vector is empty.");
  }

  static void w_mean(class context& context, std::ostream&)
  {
    const auto vec = context.pop().as_vector();
    const auto size = vec.size();

    if (size > 0)
    {
      auto sum = vec[0].as_number();

      for (std::vector<value>::size_type i = 1; i < size; ++i)
      {
        sum += vec[i].as_number();
      }
      context << value::make_number(sum / size);
      return;
    }

    throw error(error::type_range, U"Vector is empty.");
  }

  static void w_sum(class context& context, std::ostream&)
  {
    const auto vec = context.pop().as_vector();
    const auto size = vec.size();

    if (size > 0)
    {
      auto sum = vec[0].as_number();

      for (std::vector<value>::size_type i = 1; i < size; ++i)
      {
        sum += vec[i].as_number();
      }
      context << value::make_number(sum);
      return;
    }

    throw error(error::type_range, U"Vector is empty.");
  }

  static void w_for_each(class context& context, std::ostream& out)
  {
    const auto vec = context.pop().as_vector();
    const auto quote = context.pop().as_quote();

    for (const auto& value : vec)
    {
      context.push(value);
      quote.call(context, out);
    }
  }

  static void w_map(class context& context, std::ostream& out)
  {
    const auto vec = context.pop().as_vector();
    const auto quote = context.pop().as_quote();
    std::vector<value> result;

    result.reserve(vec.size());
    for (const auto& value : vec)
    {
      context.push(value);
      quote.call(context, out);
      result.push_back(context.pop());
    }
    context.push(value::make_vector(result));
  }

  static void w_filter(class context& context, std::ostream& out)
  {
    const auto vec = context.pop().as_vector();
    const auto quote = context.pop().as_quote();
    std::vector<value> result;

    for (const auto& value : vec)
    {
      context.push(value);
      quote.call(context, out);
      if (context.pop().as_boolean())
      {
        result.push_back(value);
      }
    }
    context.push(value::make_vector(result));
  }

  namespace api
  {
    extern "C" const context::dictionary_definition vector =
    {
      { U"vector:max", w_max },
      { U"vector:min", w_min },
      { U"vector:mean", w_mean },
      { U"vector:sum", w_sum },

      // Iteration.
      { U"vector:for-each", w_for_each },
      { U"vector:map", w_map },
      { U"filter", w_filter }
    };
  }
}
