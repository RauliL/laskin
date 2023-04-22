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
  static void w_vector(class context& context, std::ostream&)
  {
    const auto size = context.pop().as_number();
    std::vector<value> elements;

    elements.reserve(size.to_long());
    for (number i; i < size; ++i)
    {
      elements.push_back(context.pop());
    }

    context << value::make_vector(elements.rbegin(), elements.rend());
  }

  static void w_length(class context& context, std::ostream&)
  {
    context << value::make_number(
      mpf_class(context.peek().as_vector().size()
    ));
  }

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

    throw error(error::type::range, U"Vector is empty.");
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

    throw error(error::type::range, U"Vector is empty.");
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
        sum = sum + vec[i].as_number();
      }
      context << value::make_number(
        sum.value() / size,
        sum.measurement_unit()
      );
      return;
    }

    throw error(error::type::range, U"Vector is empty.");
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
        sum = sum + vec[i].as_number();
      }
      context << value::make_number(sum);
      return;
    }

    throw error(error::type::range, U"Vector is empty.");
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

  static void w_reduce(class context& context, std::ostream& out)
  {
    const auto vec = context.pop().as_vector();
    const auto quote = context.pop().as_quote();
    const auto size = vec.size();
    value result;

    if (!size)
    {
      throw error(error::type::range, U"Cannot reduce empty vector.");
    }
    result = vec[0];
    for (std::vector<value>::size_type i = 1; i < size; ++i)
    {
      context << result << vec[i];
      quote.call(context, out);
      context >> result;
    }
    context << result;
  }

  static void w_prepend(class context& context, std::ostream&)
  {
    auto vec = context.pop().as_vector();
    const auto value = context.pop();

    vec.insert(std::begin(vec), 1, value);
    context << value::make_vector(vec);
  }

  static void w_append(class context& context, std::ostream&)
  {
    auto vec = context.pop().as_vector();
    const auto value = context.pop();

    vec.push_back(value);
    context << value::make_vector(vec);
  }

  static void w_insert(class context& context, std::ostream&)
  {
    auto vec = context.pop().as_vector();
    const auto size = vec.size();
    const auto value = context.pop();
    auto index = context.pop().as_number().to_long();

    if (index < 0)
    {
      index += size;
    }
    if (!size || index < 0 || index >= static_cast<long>(size))
    {
      throw error(error::type::range, U"Vector index out of bounds.");
    }
    vec.insert(std::begin(vec) + index, 1, value);
    context << value::make_vector(vec);
  }

  static void w_reverse(class context& context, std::ostream&)
  {
    const auto vector = context.pop().as_vector();

    context << value::make_vector(vector.rbegin(), vector.rend());
  }

  static void w_extract(class context& context, std::ostream&)
  {
    const auto vector = context.pop().as_vector();

    for (const auto& value : vector)
    {
      context << value;
    }
  }

  static std::vector<value>::size_type partition(
    std::vector<value>& vector,
    std::vector<value>::size_type low,
    std::vector<value>::size_type high
  )
  {
    const auto& pivot = vector[high];
    auto i = low - 1;

    for (std::vector<value>::size_type j = low; j <= high - 1; ++j)
    {
      if (vector[j] < pivot)
      {
        ++i;
        std::swap(vector[i], vector[j]);
      }
    }
    std::swap(vector[i + 1], vector[high]);

    return i + 1;
  }

  static void quicksort(
    std::vector<value>& vector,
    std::vector<value>::size_type low,
    std::vector<value>::size_type high
  )
  {
    if (low < high)
    {
      const auto pi = partition(vector, low, high);

      quicksort(vector, low, pi - 1);
      quicksort(vector, pi + 1, high);
    }
  }

  static void w_sort(class context& context, std::ostream&)
  {
    auto vector = context.pop().as_vector();

    quicksort(vector, 0, vector.size() - 1);
    context << value::make_vector(vector);
  }

  static void w_at(class context& context, std::ostream&)
  {
    const auto vector = context.pop().as_vector();
    const auto size = vector.size();
    auto index = context.pop().as_number().to_long();

    if (index < 0)
    {
      index += size;
    }
    if (!size || index < 0 || index >= static_cast<long>(size))
    {
      throw error(error::type::range, U"Vector index out of bounds.");
    }
    context << vector[index];
  }

  static void w_to_time(class context& context, std::ostream&)
  {
    const auto vector = context.pop().as_vector();
    long hour;
    long minute;
    long second;

    if (vector.size() != 3)
    {
      throw error(error::type::range, U"Time needs three values.");
    }
    hour = vector[0].as_number().to_long();
    minute = vector[1].as_number().to_long();
    second = vector[2].as_number().to_long();
    if (!peelo::chrono::time::is_valid(hour, minute, second))
    {
      throw error(error::type::range, U"Invalid time.");
    }
    context << value::make_time(peelo::chrono::time(hour, minute, second));
  }

  static void w_to_date(class context& context, std::ostream&)
  {
    const auto vector = context.pop().as_vector();
    long day;
    peelo::chrono::month month;
    long year;

    if (vector.size() != 3)
    {
      throw error(error::type::range, U"Date needs three values.");
    }
    year = vector[0].as_number().to_long();
    if (vector[1].is(value::type::month))
    {
      month = vector[1].as_month();
    } else {
      const auto value = vector[1].as_number().to_long();

      if (value < 1 || value > 12)
      {
        throw error(error::type::range, U"Given month is out of range.");
      }
      month = static_cast<peelo::chrono::month>(value - 1);
    }
    day = vector[2].as_number().to_long();
    if (!peelo::chrono::date::is_valid(year, month, day))
    {
      throw error(error::type::range, U"Invalid date.");
    }
    context << value::make_date(peelo::chrono::date(year, month, day));
  }

  namespace api
  {
    extern "C" const context::dictionary_definition vector =
    {
      { U"vector", w_vector },

      { U"vector:length", w_length },

      { U"vector:max", w_max },
      { U"vector:min", w_min },
      { U"vector:mean", w_mean },
      { U"vector:sum", w_sum },

      // Iteration.
      { U"vector:for-each", w_for_each },
      { U"vector:map", w_map },
      { U"vector:filter", w_filter },
      { U"vector:reduce", w_reduce },

      // Modifications.
      { U"vector:prepend", w_prepend },
      { U"vector:append", w_append },
      { U"vector:insert", w_insert },
      { U"vector:reverse", w_reverse },
      { U"vector:extract", w_extract },
      { U"vector:sort", w_sort },

      // Element access.
      { U"vector:@", w_at },

      // Conversions.
      { U"vector:>date", w_to_date },
      { U"vector:>time", w_to_time }
    };
  }
}
