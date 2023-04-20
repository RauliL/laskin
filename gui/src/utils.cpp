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
#include "laskin/gui/utils.hpp"

namespace laskin::gui::utils
{
  void count_open_braces(
    std::stack<char32_t>& open_braces,
    const Glib::ustring& line
  )
  {
    const auto length = line.length();

    for (std::string::size_type i = 0; i < length; ++i)
    {
      const auto c = line[i];

      switch (line[i])
      {
        case '#':
          return;

        case '(':
          open_braces.push(')');
          break;

        case '[':
          open_braces.push(']');
          break;

        case ')':
        case ']':
          if (!open_braces.empty() && open_braces.top() == c)
          {
            open_braces.pop();
          }
          break;

        case '"':
        case '\'':
          ++i;
          while (i < length)
          {
            if (line[i] == c)
            {
              break;
            }
            else if (line[i] == '\\' && i + 1 < length && line[i + 1] == c)
            {
              i += 2;
            } else {
              ++i;
            }
          }
          break;
      }
    }
  }

  const Pango::FontDescription& get_monospace_font()
  {
    static Pango::FontDescription font;
    static bool initialized = false;

    if (!initialized)
    {
      font.set_family("monospace");
      initialized = true;
    }

    return font;
  }
}
