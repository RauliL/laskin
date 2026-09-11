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

#include <filesystem>
#include <iostream>
#include <optional>

#include "laskin/types.hpp"

namespace laskin
{
  class context;
  class node;

  scripted_quote parse(
    const std::u32string& source,
    const std::optional<std::filesystem::path>& path = std::nullopt,
    int line = 1,
    int column = 1
  );

  scripted_quote parse(
    const std::string& source,
    const std::optional<std::filesystem::path>& path = std::nullopt,
    int line = 1,
    int column = 1
  );

  scripted_quote parse(
    std::istream& input,
    const std::optional<std::filesystem::path>& path = std::nullopt,
    int line = 1,
    int column = 1
  );

  /**
   * Executes given quote with given execution context and optional output
   * stream.
   */
  void call(
    const quote& quote,
    context& context,
    std::ostream* out = nullptr
  );

  /**
   * Converts the quote back into source code.
   */
  std::u32string to_source(const quote& q);

  /**
   * Tests whether two quotes could be considered to be equivalent.
   */
  bool equals(const quote& a, const quote& b);

  inline bool operator==(const quote& a, const quote& b)
  {
    return equals(a, b);
  }

  inline bool operator!=(const quote& a, const quote& b)
  {
    return !equals(a, b);
  }
}
