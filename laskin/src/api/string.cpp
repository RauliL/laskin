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
#include <peelo/unicode/ctype/isspace.hpp>
#include <peelo/unicode/ctype/isupper.hpp>
#include <peelo/unicode/ctype/tolower.hpp>
#include <peelo/unicode/ctype/toupper.hpp>
#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/macros.hpp"
#include "laskin/number.hpp"

using namespace laskin;

/**
 * string:length ( string -- string number )
 *
 * Returns the number of characters in the string.
 */
BUILTIN_WORD(w_length)
{
  context << value::make_number(
    static_cast<std::int64_t>(context.peek().as_string().length())
  );
}

/**
 * string:chars ( string -- string vector )
 *
 * Extracts characters from string and returns them in vector of
 * substrings.
 */
BUILTIN_WORD(w_chars)
{
  const auto str = context.peek().as_string();
  value::vector_container result;

  result.reserve(str.length());
  for (const auto& c : str)
  {
    result.push_back(value::make_string(std::u32string(&c, 1)));
  }
  context << result;
}

/**
 * string:runes ( string -- string vector )
 *
 * Extracts all Unicode code points of characters in the string and returns
 * them as numbers inside vector.
 */
BUILTIN_WORD(w_runes)
{
  const auto str = context.peek().as_string();
  value::vector_container result;

  result.reserve(str.length());
  for (const auto& c : str)
  {
    result.push_back(value::make_number(static_cast<std::int64_t>(c)));
  }
  context << result;
}

/**
 * string:words ( string -- string vector )
 *
 * Extracts all whitespace separated words from the string and returns them
 * in vector.
 */
BUILTIN_WORD(w_words)
{
  const auto str = context.peek().as_string();
  const auto length = str.length();
  std::u32string::size_type begin = 0;
  std::u32string::size_type end = 0;
  value::vector_container result;

  for (std::u32string::size_type i = 0; i < length; ++i)
  {
    if (peelo::unicode::ctype::isspace(str[i]))
    {
      if (end - begin > 0)
      {
        result.push_back(value::make_string(str.substr(begin, end - begin)));
      }
      begin = end = i + 1;
    } else {
      ++end;
    }
  }
  if (end - begin > 0)
  {
    result.push_back(value::make_string(str.substr(begin, end - begin)));
  }
  context << result;
}

/**
 * string:lines ( string -- string vector )
 *
 * Extracts all lines from the string and returns them in vector.
 */
BUILTIN_WORD(w_lines)
{
  const auto str = context.peek().as_string();
  const auto length = str.length();
  std::u32string::size_type begin = 0;
  std::u32string::size_type end = 0;
  value::vector_container result;

  for (std::u32string::size_type i = 0; i < length; ++i)
  {
    const auto& c = str[i];

    if (i + 1 < length && c == '\r' && str[i + 1] == '\n')
    {
      result.push_back(value::make_string(str.substr(begin, end - begin)));
      begin = end = ++i + 1;
    }
    else if (c == '\n' || c == '\r')
    {
      result.push_back(value::make_string(str.substr(begin, end - begin)));
      begin = end = i + 1;
    } else {
      ++end;
    }
  }
  if (end - begin > 0)
  {
    result.push_back(value::make_string(str.substr(begin, end - begin)));
  }
  context << result;
}

/**
 * string:starts-with? ( string string -- boolean )
 *
 * Tests whether string has the same prefix than the other string.
 */
BUILTIN_WORD(w_starts_with)
{
  const auto string = context.pop().as_string();
  const auto substring = context.pop().as_string();
  const auto string_length = string.length();
  const auto substring_length = substring.length();

  if (substring_length > string_length)
  {
    context << false;
    return;
  }
  else if (!substring_length)
  {
    context << true;
    return;
  }

  for (std::u32string::size_type i = 0; i < substring_length; ++i)
  {
    if (string[i] != substring[i])
    {
      context << false;
      return;
    }
  }

  context << true;
}

/**
 * string:ends-with? ( string string -- boolean )
 *
 * Tests whether string has the same suffix than the other string.
 */
BUILTIN_WORD(w_ends_with)
{
  const auto string = context.pop().as_string();
  const auto substring = context.pop().as_string();
  const auto string_length = string.length();
  const auto substring_length = substring.length();

  if (substring_length > string_length)
  {
    context << false;
    return;
  }
  else if (!substring_length)
  {
    context << true;
    return;
  }

  for (std::u32string::size_type i = 0; i < substring_length; ++i)
  {
    if (string[string_length - substring_length + i] != substring[i])
    {
      context << false;
      return;
    }
  }

  context << true;
}

/**
 * string:includes? ( string string -- boolean )
 *
 * Tests whether string contains the other string somewhere in it's contents.
 */
BUILTIN_WORD(w_includes)
{
  const auto string = context.pop().as_string();
  const auto substring = context.pop().as_string();
  const auto string_length = string.length();
  const auto substring_length = substring.length();
  std::u32string::size_type position;

  if (substring_length > string_length)
  {
    context << false;
    return;
  }
  else if (!substring_length)
  {
    context << true;
    return;
  }

  position = string.find(substring);
  context << (position != std::u32string::npos);
}

/**
 * string:index-of ( string string -- boolean|number )
 *
 * Finds first position of substring from a string and returns it's index, or
 * false if the substring does not exist in the string.
 */
BUILTIN_WORD(w_index_of)
{
  const auto string = context.pop().as_string();
  const auto substring = context.pop().as_string();
  const auto string_length = string.length();
  const auto substring_length = substring.length();
  std::u32string::size_type position;

  if (substring_length > string_length)
  {
    context << false;
    return;
  }
  else if (!substring_length)
  {
    context << 0;
    return;
  }

  position = string.find(substring);
  if (position == std::u32string::npos)
  {
    context << false;
    return;
  }

  context << value::make_number(static_cast<std::int64_t>(position));
}

/**
 * string:last-index-of ( string string -- boolean|number )
 *
 * Finds last position of substring from a string and returns it's index, or
 * false if the substring does not exist in the string.
 */
BUILTIN_WORD(w_last_index_of)
{
  const auto string = context.pop().as_string();
  const auto substring = context.pop().as_string();
  const auto string_length = string.length();
  const auto substring_length = substring.length();
  std::u32string::size_type position;

  if (substring_length > string_length)
  {
    context << false;
    return;
  }
  else if (!substring_length)
  {
    context << 0;
    return;
  }

  position = string.rfind(substring);
  if (position == std::u32string::npos)
  {
    context << false;
    return;
  }

  context << value::make_number(static_cast<std::int64_t>(position));
}

/**
 * string:reverse ( string -- string )
 *
 * Returns reversed copy of the string.
 */
BUILTIN_WORD(w_reverse)
{
  const auto string = context.pop().as_string();

  context << value::make_string(string.rbegin(), string.rend());
}

static void
convert_string(class context& context, char32_t (*callback)(char32_t))
{
  auto string = context.pop().as_string();

  for (auto& c : string)
  {
    c = callback(c);
  }
  context << string;
}

/**
 * string:lower-case ( string -- string )
 *
 * Converts all upper case characters into lower case.
 */
BUILTIN_WORD(w_lower_case)
{
  convert_string(context, peelo::unicode::ctype::tolower);
}

/**
 * string:upper-case ( string -- string )
 *
 * Converts all lower case characters into upper case.
 */
BUILTIN_WORD(w_upper_case)
{
  convert_string(context, peelo::unicode::ctype::toupper);
}

/**
 * string:swap-case ( string -- string )
 *
 * Converts all lower case characters into upper case and vice versa.
 */
BUILTIN_WORD(w_swap_case)
{
  using peelo::unicode::ctype::isupper;
  using peelo::unicode::ctype::tolower;
  using peelo::unicode::ctype::toupper;

  convert_string(
    context,
    [](char32_t c)
    {
      return isupper(c) ? tolower(c) : toupper(c);
    }
  );
}

/**
 * string:trim ( string -- string )
 *
 * Removes whitespace from beginning and end of the string.
 */
BUILTIN_WORD(w_trim)
{
  const auto string = context.pop().as_string();
  const auto length = string.length();
  std::u32string::size_type i, j;

  for (i = 0; i < length; ++i)
  {
    if (!peelo::unicode::ctype::isspace(string[i]))
    {
      break;
    }
  }
  for (j = length; j != 0; --j)
  {
    if (!peelo::unicode::ctype::isspace(string[j - 1]))
    {
      break;
    }
  }

  if (i != 0 || j != length)
  {
    context << string.substr(i, j - i);
  } else {
    context << string;
  }
}

/**
 * string:trim-start ( string -- string )
 *
 * Removes whitespace from beginning of the string.
 */
BUILTIN_WORD(w_trim_start)
{
  const auto string = context.pop().as_string();
  const auto length = string.length();
  std::u32string::size_type i;

  for (i = 0; i < length; ++i)
  {
    if (!peelo::unicode::ctype::isspace(string[i]))
    {
      break;
    }
  }

  if (i != 0)
  {
    context << string.substr(i, length - i);
  } else {
    context << string;
  }
}

/**
 * string:trim-end ( string -- string )
 *
 * Removes whitespace from the end of the string.
 */
BUILTIN_WORD(w_trim_end)
{
  const auto string = context.pop().as_string();
  const auto length = string.length();
  std::u32string::size_type i;

  for (i = length; i != 0; --i)
  {
    if (!peelo::unicode::ctype::isspace(string[i - 1]))
    {
      break;
    }
  }

  if (i != length)
  {
    context << string.substr(0, i);
  } else {
    context << string;
  }
}

/**
 * string:substring ( number number string -- string )
 *
 * Returns portion of the string based on given begin and end index. Negative
 * indices are supported.
 *
 * Range error will be thrown if either of the indices are out of bounds.
 */
BUILTIN_WORD(w_substring)
{
  try
  {
    const auto string = context.pop().as_string();
    const auto length = string.length();
    auto begin = long(context.pop().as_number());
    auto end = long(context.pop().as_number());

    if (begin < 0)
    {
      begin += length;
    }
    if (end < 0)
    {
      end += length;
    }

    if (
      (begin < 0 || begin >= static_cast<std::int64_t>(length)) ||
        (end < 0 || end >= static_cast<std::int64_t>(length)) ||
        end < begin)
    {
      throw error(error::type::range, U"String index out of bounds.");
    }

    context << string.substr(begin, end - begin + 1);
  }
  catch (const std::underflow_error&)
  {
    throw error(error::type::range, U"Numeric underflow.");
  }
  catch (const std::overflow_error&)
  {
    throw error(error::type::range, U"Numeric overflow.");
  }
}

/**
 * string:split ( string string -- vector )
 *
 * Splits the string into substrings based on pattern given as another
 * string.
 */
BUILTIN_WORD(w_split)
{
  const auto string = context.pop().as_string();
  const auto pattern = context.pop().as_string();
  const auto string_length = string.length();
  const auto pattern_length = pattern.length();
  value::vector_container result;

  if (pattern_length)
  {
    std::u32string::size_type begin = 0;
    std::u32string::size_type end = 0;

    for (std::u32string::size_type i = 0; i < string_length; ++i)
    {
      bool found = true;

      for (std::u32string::size_type j = 0;
            j < pattern_length && i + j < string_length;
            ++j)
      {
        if (pattern[j] != string[i + j])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        result.push_back(
          value::make_string(string.substr(begin, end - begin))
        );
        begin = end = i + 1;
      } else {
        ++end;
      }
    }
    if (end - begin > 0)
    {
      result.push_back(
        value::make_string(string.substr(begin, end - begin))
      );
    }
  } else {
    result.reserve(string_length);
    for (std::u32string::size_type i = 0; i < string_length; ++i)
    {
      result.push_back(value::make_string(std::u32string(&string[i], 1)));
    }
  }

  context << result;
}

/**
 * string:repeat ( number string -- string )
 *
 * Repeats string given number of times.
 */
BUILTIN_WORD(w_repeat)
{
  try
  {
    const auto string = context.pop().as_string();
    auto count = long(context.pop().as_number());
    std::u32string result;

    result.reserve(string.length() * count);
    while (count > 0)
    {
      result.append(string);
      --count;
    }
    context << result;
  }
  catch (const std::underflow_error&)
  {
    throw error(error::type::range, U"Numeric underflow.");
  }
  catch (const std::overflow_error&)
  {
    throw error(error::type::range, U"Numeric overflow.");
  }
}

/**
 * string:replace ( string string string -- string )
 *
 * Finds a substring from a string and replaces it with given replacement
 * returning result.
 */
BUILTIN_WORD(w_replace)
{
  const auto string = context.pop().as_string();
  const auto replacement = context.pop().as_string();
  const auto needle = context.pop().as_string();
  const auto string_length = string.length();
  const auto needle_length = needle.length();
  std::u32string result;

  for (std::u32string::size_type i = 0; i < string_length; ++i)
  {
    bool found = true;

    for (std::u32string::size_type j = 0; j < needle_length; ++j)
    {
      if (i + j >= string_length || string[i + j] != needle[j])
      {
        found = false;
        break;
      }
    }
    if (found)
    {
      result.append(replacement);
      result.append(string.substr(i + needle_length));
      break;
    }
    result.append(1, string[i]);
  }

  context << result;
}

/**
 * string:pad-start ( number string string -- string )
 *
 * Makes sure that a string is long enough by adding a padding string to it's
 * start required number of times.
 */
BUILTIN_WORD(w_pad_start)
{
  try
  {
    const auto string = context.pop().as_string();
    auto pad_string = context.pop().as_string();
    auto target_length = long(context.pop().as_number());
    const auto string_length = string.length();
    const auto pad_string_length = pad_string.length();

    if (static_cast<long>(string_length) >= target_length)
    {
      context << string;
      return;
    }

    target_length -= string_length;
    if (target_length > static_cast<long>(pad_string_length))
    {
      const auto original_pad_string = pad_string;
      auto count = target_length / pad_string_length;

      while (count-- > 0)
      {
        pad_string += original_pad_string;
      }
    }

    context << (pad_string.substr(0, target_length) + string);
  }
  catch (const std::underflow_error&)
  {
    throw error(error::type::range, U"Numeric underflow.");
  }
  catch (const std::overflow_error&)
  {
    throw error(error::type::range, U"Numeric overflow.");
  }
}

/**
 * string:pad-end ( number string string -- string )
 *
 * Makes sure that a string is long enough by adding a padding string to it's
 * end required number of times.
 */
BUILTIN_WORD(w_pad_end)
{
  try
  {
    const auto string = context.pop().as_string();
    auto pad_string = context.pop().as_string();
    auto target_length = long(context.pop().as_number());
    const auto string_length = string.length();
    const auto pad_string_length = pad_string.length();

    if (static_cast<long>(string_length) >= target_length)
    {
      context << string;
      return;
    }

    target_length -= string_length;
    if (target_length > static_cast<long>(pad_string_length))
    {
      const auto original_pad_string = pad_string;
      auto count = target_length / pad_string_length;

      while (count-- > 0)
      {
        pad_string += original_pad_string;
      }
    }

    context << (string + pad_string.substr(0, target_length));
  }
  catch (const std::underflow_error&)
  {
    throw error(error::type::range, U"Numeric underflow.");
  }
  catch (const std::overflow_error&)
  {
    throw error(error::type::range, U"Numeric overflow.");
  }
}

/**
 * string:@ ( number string -- string )
 *
 * Picks a character from given index and returns it as a string. Negative
 * indices are supported.
 *
 * Range error will be thrown if the index is out of bounds.
 */
BUILTIN_WORD(w_at)
{
  try
  {
    const auto string = context.pop().as_string();
    const auto length = string.length();
    auto index = long(context.pop().as_number());
    char32_t c;

    if (index < 0)
    {
      index += length;
    }
    if (!length || index < 0 || index >= static_cast<long>(length))
    {
      throw error(error::type::range, U"String index out of bounds.");
    }
    c = string[index];
    context << std::u32string(&c, 1);
  }
  catch (const std::underflow_error&)
  {
    throw error(error::type::range, U"Numeric underflow.");
  }
  catch (const std::overflow_error&)
  {
    throw error(error::type::range, U"Numeric overflow.");
  }
}

/**
 * string:>number ( string -- number )
 *
 * Parses string as number and returns result.
 *
 * Range error will be thrown if the string cannot be parsed as number.
 */
BUILTIN_WORD(w_to_number)
{
  using peelo::unicode::encoding::utf8::encode;

  const auto string = context.pop().as_string();

  if (!is_number(string))
  {
    throw error(
      error::type::range,
      U"Cannot convert given string into a number."
    );
  }
  context << peelo::number::parse(encode(string));
}

/**
 * string:>quote ( string -- quote )
 *
 * Parses string as Laskin program and turns it into an executable quote.
 */
BUILTIN_WORD(w_to_quote)
{
  const auto source = context.pop().as_string();

  context << quote::parse(source);
}

namespace laskin::api
{
  extern "C" const context::dictionary_definition string =
  {
    { U"string:length", w_length },
    { U"string:chars", w_chars },
    { U"string:runes", w_runes },
    { U"string:words", w_words },
    { U"string:lines", w_lines },

    // Comparison and lookup.
    { U"string:starts-with?", w_starts_with },
    { U"string:ends-with?", w_ends_with },
    { U"string:includes?", w_includes },
    { U"string:index-of", w_index_of },
    { U"string:last-index-of", w_last_index_of },

    // Modifications.
    { U"string:reverse", w_reverse },
    { U"string:lower-case", w_lower_case },
    { U"string:upper-case", w_upper_case },
    { U"string:swap-case", w_swap_case },
    { U"string:trim", w_trim },
    { U"string:trim-start", w_trim_start },
    { U"string:trim-end", w_trim_end },
    { U"string:substring", w_substring },
    { U"string:split", w_split },
    { U"string:repeat", w_repeat },
    { U"string:replace", w_replace },
    { U"string:pad-start", w_pad_start },
    { U"string:pad-end", w_pad_end },

    { U"string:@", w_at },

    // Conversions.
    { U">number", w_to_number },
    { U">quote", w_to_quote }
  };
}
