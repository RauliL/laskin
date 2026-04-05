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
#pragma once

#include <exception>

#include "laskin/position.hpp"

namespace laskin
{
  class error : public std::exception
  {
  public:
    enum class type
    {
      domain,
      exit,
      name,
      range,
      syntax,
      system,
      type,
      unit,
    };

    enum type type;
    std::string message;
    std::optional<struct position> position;

    explicit error(
      enum type type_,
      const std::u32string& message = std::u32string(),
      const std::optional<struct position>& position_ = std::nullopt
    );

    error(const error&) = default;
    error(error&&) = default;
    error& operator=(const error& that) = default;
    error& operator=(error&&) = default;

    inline bool is(enum type t) const
    {
      return type == t;
    }

    inline const char* what() const noexcept
    {
      return message.c_str();
    }
  };

  std::ostream& operator<<(std::ostream&, enum error::type);

  std::ostream& operator<<(std::ostream&, const error&);
}
