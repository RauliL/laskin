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
#ifndef LASKIN_ERROR_HPP_GUARD
#define LASKIN_ERROR_HPP_GUARD

#include <exception>
#include <iostream>
#include <string>

namespace laskin
{
  class error : public std::exception
  {
  public:
    enum type
    {
      type_syntax,
      type_type,
      type_range,
      type_name
    };

    explicit error(
      enum type type,
      const std::u32string& message,
      int line = 0,
      int column = 0
    );

    error(const error& that);
    error& operator=(const error& that);

    inline enum type type() const
    {
      return m_type;
    }

    inline bool is(enum type type) const
    {
      return m_type == type;
    }

    inline const std::string& message() const
    {
      return m_message;
    }

    inline const char* what() const noexcept
    {
      return m_message.c_str();
    }

    inline int line() const
    {
      return m_line;
    }

    inline int column() const
    {
      return m_column;
    }

    static std::u32string type_description(enum type type);

  private:
    enum type m_type;
    std::string m_message;
    int m_line;
    int m_column;
  };

  std::ostream& operator<<(std::ostream&, const error&);
}

#endif /* !LASKIN_ERROR_HPP_GUARD */
