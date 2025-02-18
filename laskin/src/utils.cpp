/*
 * Copyright (c) 2018-2020, Rauli Laine
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
#include <peelo/unicode/ctype/isprint.hpp>

#include "laskin/utils.hpp"

namespace laskin::utils
{
  std::int64_t time_as_seconds(const peelo::chrono::time& time)
  {
    const auto hour = static_cast<std::int64_t>(time.hour());
    const auto minute = static_cast<std::int64_t>(time.minute());
    auto second = static_cast<std::int64_t>(time.second());

    if (minute > 0)
    {
      second += minute * 60;
    }
    if (hour > 0)
    {
      second += hour * 3600;
    }

    return second;
  }

  std::u32string escape_string(const std::u32string& str)
  {
    std::u32string result;

    result.reserve(str.length() + 2);
    result.append(1, '"');

    for (const auto& c : str)
    {
      switch (c)
      {
        case 010:
          result.append(1, '\\');
          result.append(1, 'b');
          break;

        case 011:
          result.append(1, '\\');
          result.append(1, 't');
          break;

        case 012:
          result.append(1, '\\');
          result.append(1, 'n');
          break;

        case 014:
          result.append(1, '\\');
          result.append(1, 'f');
          break;

        case 015:
          result.append(1, '\\');
          result.append(1, 'r');
          break;

        case '"':
        case '\\':
        case '/':
          result.append(1, '\\');
          result.append(1, c);
          break;

        default:
          if (!peelo::unicode::ctype::isprint(c))
          {
            char buffer[7];

            std::snprintf(buffer, 7, "\\u%04x", c);
            for (const char* p = buffer; *p; ++p)
            {
              result.append(1, static_cast<char32_t>(*p));
            }
          } else {
            result.append(1, c);
          }
      }
    }

    result.append(1, '"');

    return result;
  }
}
