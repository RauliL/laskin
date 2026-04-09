/*
 * Copyright (c) 2026, Rauli Laine
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
#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace laskin2cpp
{
  class writer
  {
  public:
    using value_type = std::string;
    using container_type = std::vector<value_type>;
    using const_iterator = container_type::const_iterator;

    static std::string escape(const std::u32string& input);

    writer()
      : m_indent(2) {}

    writer(const writer&) = default;
    writer(writer&&) = default;
    writer& operator=(const writer&) = default;
    writer& operator=(writer&&) = default;

    inline const_iterator begin() const
    {
      return std::begin(m_lines);
    }

    inline const_iterator end() const
    {
      return std::end(m_lines);
    }

    inline void indent()
    {
      ++m_indent;
    }

    inline void dedent()
    {
      --m_indent;
    }

    void flush();
    void print(const value_type& line);
    void println(const value_type& line);

  private:
    value_type m_buffer;
    container_type m_lines;
    int m_indent;
  };

  std::ostream& operator<<(std::ostream&, const writer&);
}
