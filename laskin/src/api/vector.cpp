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
#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/macros.hpp"

using namespace laskin;

/**
 * vector ( number -- vector )
 *
 * Constructs vector from given number of values extracted from the stack.
 */
BUILTIN_WORD(w_vector)
{
  const auto size = context.pop().as_number();
  std::vector<value> elements;

  elements.reserve(long(size));
  for (number i; i < size; ++i)
  {
    elements.push_back(context.pop());
  }

  context << value::make_vector(elements.rbegin(), elements.rend());
}

/**
 * vector:length ( vector -- vector number )
 *
 * Returns the number of elements in the vector.
 */
BUILTIN_WORD(w_length)
{
  context << value::make_number(
    static_cast<std::int64_t>(context.peek().as_vector().size())
  );
}

/**
 * vector:max ( vector -- any )
 *
 * Returns the maximum value contained in the vector.
 *
 * Range error is thrown if the vector is empty.
 */
BUILTIN_WORD(w_max)
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

/**
 * vector:min ( vector -- any )
 *
 * Returns the minimum value contained in the vector.
 *
 * Range error is thrown if the vector is empty.
 */
BUILTIN_WORD(w_min)
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

/**
 * vector:mean ( vector -- any )
 *
 * Returns the mean value contained in the vector.
 *
 * Range error is thrown if the vector is empty.
 */
BUILTIN_WORD(w_mean)
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
    context << (sum / static_cast<std::int64_t>(size));
    return;
  }

  throw error(error::type::range, U"Vector is empty.");
}

/**
 * vector:sum ( vector -- any )
 *
 * Sums all values inside the vector together.
 *
 * Range error is thrown if the vector is empty.
 */
BUILTIN_WORD(w_sum)
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
    context << sum;
    return;
  }

  throw error(error::type::range, U"Vector is empty.");
}

/**
 * vector:for-each ( quote vector -- )
 *
 * Executes quote once for each value in the vector.
 */
BUILTIN_WORD(w_for_each)
{
  const auto vec = context.pop().as_vector();
  const auto quote = context.pop().as_quote();

  for (const auto& value : vec)
  {
    context.push(value);
    quote.call(context, out);
  }
}

/**
 * vector:map ( quote vector -- vector )
 *
 * Constructs new vector from results of what executing the given quote once
 * for every value in the vector will return.
 */
BUILTIN_WORD(w_map)
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
  context << result;
}

/**
 * vector:filter ( quote vector -- vector )
 *
 * Constructs new vector by filtering the existing one by executing the quote
 * once for every value in the vector, leaving out those for which the quote
 * returns false.
 */
BUILTIN_WORD(w_filter)
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
  context << result;
}

/**
 * vector:reduce ( quote vector -- any )
 *
 * Performs reduce operation on the vector by calling the quote once for
 * every other value in the vector except the first one.
 *
 * Range error will be thrown if the vector is empty.
 */
BUILTIN_WORD(w_reduce)
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

/**
 * vector:prepend ( any vector -- vector )
 *
 * Constructs new vector from the one given with the new value prepended into
 * it.
 */
BUILTIN_WORD(w_prepend)
{
  auto vec = context.pop().as_vector();
  const auto value = context.pop();

  vec.insert(std::begin(vec), 1, value);
  context << vec;
}

/**
 * vector:append ( any vector -- vector )
 *
 * Constructs new vector from the one given with the new value appended into
 * it.
 */
BUILTIN_WORD(w_append)
{
  auto vec = context.pop().as_vector();
  const auto value = context.pop();

  vec.push_back(value);
  context << vec;
}

/**
 * vector:insert ( any number vector -- vector )
 *
 * Inserts new value at the given index. Negative indices are supported.
 *
 * Throws range error if given index is out of bounds.
 */
BUILTIN_WORD(w_insert)
{
  auto vec = context.pop().as_vector();
  const auto size = vec.size();
  const auto value = context.pop();
  auto index = long(context.pop().as_number());

  if (index < 0)
  {
    index += size;
  }
  if (!size || index < 0 || index >= static_cast<long>(size))
  {
    throw error(error::type::range, U"Vector index out of bounds.");
  }
  vec.insert(std::begin(vec) + index, 1, value);
  context << vec;
}

/**
 * vector:reverse ( vector -- vector )
 *
 * Returns reversed copy of the vector.
 */
BUILTIN_WORD(w_reverse)
{
  const auto vector = context.pop().as_vector();

  context << value::make_vector(vector.rbegin(), vector.rend());
}

/**
 * vector:extract ( vector -- any... )
 *
 * Pushes all values inside the vector onto the stack.
 */
BUILTIN_WORD(w_extract)
{
  const auto vector = context.pop().as_vector();

  for (const auto& value : vector)
  {
    context << value;
  }
}

static std::vector<value>::size_type
partition(
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

static void
quicksort(
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

/**
 * vector:sort ( vector -- vector )
 *
 * Performs quicksort on the vector.
 */
BUILTIN_WORD(w_sort)
{
  auto vector = context.pop().as_vector();

  quicksort(vector, 0, vector.size() - 1);
  context << vector;
}

/**
 * vector:@ ( number vector -- any )
 *
 * Extracts an value from given index from the vector. Negative indices are
 * supported.
 *
 * Range error will be thrown if given index is out of bounds.
 */
BUILTIN_WORD(w_at)
{
  const auto vector = context.pop().as_vector();
  const auto size = vector.size();
  auto index = long(context.pop().as_number());

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

/**
 * vector:@= ( any number vector -- vector )
 *
 * Constructs new vector by replacing value in the existing one on given
 * index. Negative indices are supported.
 *
 * Range error will be thrown if given index is out of bounds.
 */
BUILTIN_WORD(w_set)
{
  auto vector = context.pop().as_vector();
  const auto size = vector.size();
  auto index = long(context.pop().as_number());
  const auto value = context.pop();

  if (index < 0)
  {
    index += size;
  }
  if (!size || index < 0 || index >= static_cast<long>(size))
  {
    throw error(error::type::range, U"Vector index out of bounds.");
  }
  vector[index] = value;
  context << vector;
}

/**
 * vector:>time ( vector -- time )
 *
 * Assumes that the vector contains time data (hour, minute, second) and
 * constructs a time value from it.
 *
 * Range error will be thrown if the values inside the vector cannot be used
 * to construct a time.
 */
BUILTIN_WORD(w_to_time)
{
  const auto vector = context.pop().as_vector();
  long hour;
  long minute;
  long second;

  if (vector.size() != 3)
  {
    throw error(error::type::range, U"Time needs three values.");
  }
  hour = long(vector[0].as_number());
  minute = long(vector[1].as_number());
  second = long(vector[2].as_number());
  if (!peelo::chrono::time::is_valid(hour, minute, second))
  {
    throw error(error::type::range, U"Invalid time.");
  }
  context << value::make_time(peelo::chrono::time(hour, minute, second));
}

/**
 * vector:>date ( vector -- date )
 *
 * Assumes that the vector contains date data (year, month, day of month) and
 * constructs an date from it.
 *
 * Range error will be thrown if the values inside the vector cannot be used
 * to construct a date.
 */
BUILTIN_WORD(w_to_date)
{
  const auto vector = context.pop().as_vector();
  long day;
  peelo::chrono::month month;
  long year;

  if (vector.size() != 3)
  {
    throw error(error::type::range, U"Date needs three values.");
  }
  year = long(vector[0].as_number());
  if (vector[1].is(value::type::month))
  {
    month = vector[1].as_month();
  } else {
    const auto value = long(vector[1].as_number());

    if (value < 1 || value > 12)
    {
      throw error(error::type::range, U"Given month is out of range.");
    }
    month = static_cast<peelo::chrono::month>(value - 1);
  }
  day = long(vector[2].as_number());
  if (!peelo::chrono::date::is_valid(year, month, day))
  {
    throw error(error::type::range, U"Invalid date.");
  }
  context << value::make_date(peelo::chrono::date(year, month, day));
}

namespace laskin::api
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
    { U"vector:@=", w_set },

    // Conversions.
    { U"vector:>date", w_to_date },
    { U"vector:>time", w_to_time }
  };
}
