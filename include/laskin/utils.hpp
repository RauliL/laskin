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
#ifndef LASKIN_UTILS_HPP_GUARD
#define LASKIN_UTILS_HPP_GUARD

#include <peelo/unicode/ctype.hpp>

namespace laskin
{
  namespace utils
  {
    /**
     * Tests whether given string is blank or not. String is considered to be
     * blank when it's either empty or contains only whitespace characters.
     */
    template<class T>
    inline bool is_blank(const std::basic_string<T>& str)
    {
      const auto length = str.length();

      if (!length)
      {
        return true;
      }
      for (std::size_t i = 0; i < length; ++i)
      {
        if (!peelo::unicode::isspace(str[i]))
        {
          return false;
        }
      }

      return true;
    }
  }
}

#endif /* !LASKIN_UTILS_HPP_GUARD */
