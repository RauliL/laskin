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
#include <peelo/unicode/ctype/isspace.hpp>
#include <peelo/unicode/ctype/isupper.hpp>
#include <peelo/unicode/ctype/tolower.hpp>
#include <peelo/unicode/ctype/toupper.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"

namespace laskin
{
  static void w_length(class context& context, std::ostream&)
  {
    context << value::make_number(
      mpf_class(context.peek().as_string().length())
    );
  }

  static void w_starts_with(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    const auto substring = context.pop().as_string();
    const auto string_length = string.length();
    const auto substring_length = substring.length();

    if (substring_length > string_length)
    {
      context << value::make_boolean(false);
      return;
    }
    else if (!substring_length)
    {
      context << value::make_boolean(true);
      return;
    }

    for (std::u32string::size_type i = 0; i < substring_length; ++i)
    {
      if (string[i] != substring[i])
      {
        context << value::make_boolean(false);
        return;
      }
    }

    context << value::make_boolean(true);
  }

  static void w_ends_with(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    const auto substring = context.pop().as_string();
    const auto string_length = string.length();
    const auto substring_length = substring.length();

    if (substring_length > string_length)
    {
      context << value::make_boolean(false);
      return;
    }
    else if (!substring_length)
    {
      context << value::make_boolean(true);
      return;
    }

    for (std::u32string::size_type i = 0; i < substring_length; ++i)
    {
      if (string[string_length - substring_length + i] != substring[i])
      {
        context << value::make_boolean(false);
        return;
      }
    }

    context << value::make_boolean(true);
  }

  static void w_includes(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    const auto substring = context.pop().as_string();
    const auto string_length = string.length();
    const auto substring_length = substring.length();
    std::u32string::size_type position;

    if (substring_length > string_length)
    {
      context << value::make_boolean(false);
      return;
    }
    else if (!substring_length)
    {
      context << value::make_boolean(true);
      return;
    }

    position = string.find(substring);
    context << value::make_boolean(position != std::u32string::npos);
  }

  static void w_index_of(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    const auto substring = context.pop().as_string();
    const auto string_length = string.length();
    const auto substring_length = substring.length();
    std::u32string::size_type position;

    if (substring_length > string_length)
    {
      context << value::make_boolean(false);
      return;
    }
    else if (!substring_length)
    {
      context << value::make_number(number::value_type(0));
      return;
    }

    position = string.find(substring);
    if (position == std::u32string::npos)
    {
      context << value::make_boolean(false);
      return;
    }

    context << value::make_number(static_cast<std::int64_t>(position));
  }

  static void w_last_index_of(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    const auto substring = context.pop().as_string();
    const auto string_length = string.length();
    const auto substring_length = substring.length();
    std::u32string::size_type position;

    if (substring_length > string_length)
    {
      context << value::make_boolean(false);
      return;
    }
    else if (!substring_length)
    {
      context << value::make_number(number::value_type(0));
      return;
    }

    position = string.rfind(substring);
    if (position == std::u32string::npos)
    {
      context << value::make_boolean(false);
      return;
    }

    context << value::make_number(static_cast<std::int64_t>(position));
  }

  static void w_reverse(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();

    context << value::make_string(string.rbegin(), string.rend());
  }

  static void convert_string(class context& context,
                             char32_t (*callback)(char32_t))
  {
    auto string = context.pop().as_string();

    for (auto& c : string)
    {
      c = callback(c);
    }
    context << value::make_string(string);
  }

  static void w_lower_case(class context& context, std::ostream&)
  {
    convert_string(context, peelo::unicode::ctype::tolower);
  }

  static void w_upper_case(class context& context, std::ostream&)
  {
    convert_string(context, peelo::unicode::ctype::toupper);
  }

  static void w_swap_case(class context& context, std::ostream&)
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

  static void w_trim(class context& context, std::ostream&)
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
      context << value::make_string(string.substr(i, j - i));
    } else {
      context << value::make_string(string);
    }
  }

  static void w_trim_start(class context& context, std::ostream&)
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
      context << value::make_string(string.substr(i, length - i));
    } else {
      context << value::make_string(string);
    }
  }

  static void w_trim_end(class context& context, std::ostream&)
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
      context << value::make_string(string.substr(0, i));
    } else {
      context << value::make_string(string);
    }
  }

  static void w_substring(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    const auto length = string.length();
    auto begin = context.pop().as_number().to_long();
    auto end = context.pop().as_number().to_long();

    if (begin < 0)
    {
      begin += length;
    }
    if (end < 0)
    {
      end += length;
    }

    if ((begin < 0 || begin >= static_cast<std::int64_t>(length)) ||
        (end < 0 || end >= static_cast<std::int64_t>(length)) ||
        end < begin)
    {
      throw error(error::type::range, U"String index out of bounds.");
    }

    context << value::make_string(string.substr(begin, end - begin + 1));
  }

  static void w_split(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    const auto pattern = context.pop().as_string();
    const auto string_length = string.length();
    const auto pattern_length = pattern.length();
    std::vector<value> result;

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

    context << value::make_vector(result);
  }

  static void w_repeat(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    auto count = context.pop().as_number().to_long();
    std::u32string result;

    result.reserve(string.length() * count);
    while (count > 0)
    {
      result.append(string);
      --count;
    }
    context << value::make_string(result);
  }

  static void w_replace(class context& context, std::ostream&)
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

    context << value::make_string(result);
  }

  static void w_pad_start(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    auto pad_string = context.pop().as_string();
    auto target_length = context.pop().as_number().to_long();
    const auto string_length = string.length();
    const auto pad_string_length = pad_string.length();

    if (static_cast<long>(string_length) >= target_length)
    {
      context << value::make_string(string);
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

    context << value::make_string(
      pad_string.substr(0, target_length) + string
    );
  }

  static void w_pad_end(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    auto pad_string = context.pop().as_string();
    auto target_length = context.pop().as_number().to_long();
    const auto string_length = string.length();
    const auto pad_string_length = pad_string.length();

    if (static_cast<long>(string_length) >= target_length)
    {
      context << value::make_string(string);
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

    context << value::make_string(
      string + pad_string.substr(0, target_length)
    );
  }

  static void w_at(class context& context, std::ostream&)
  {
    const auto string = context.pop().as_string();
    const auto length = string.length();
    auto index = context.pop().as_number().to_long();
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
    context << value::make_string(std::u32string(&c, 1));
  }

  namespace api
  {
    extern "C" const context::dictionary_definition string =
    {
      { U"string:length", w_length },

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

      { U"string:@", w_at }
    };
  }
}
