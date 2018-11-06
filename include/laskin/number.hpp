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
#ifndef LASKIN_NUMBER_HPP_GUARD
#define LASKIN_NUMBER_HPP_GUARD

#include <iostream>

#include <gmpxx.h>

#include "laskin/unit.hpp"

namespace laskin
{
  using number = std::pair<mpf_class, std::optional<unit>>;

  /**
   * Tests whether given string contains a valid number.
   */
  bool is_number(const std::u32string&);

  /**
   * Parses given string into a number.
   */
  number parse_number(const std::u32string&);

  /**
   * Converts given number into C++ long integer, or throws an exception if the
   * number is too large for that data type.
   */
  long to_long(const number&);

  /**
   * Converts given number into C++ double precision, or throws an exception if
   * the number is too large for that data type.
   */
  double to_double(const number&);

  number operator+(const number&, const number&);
  number operator-(const number&, const number&);
  number operator*(const number&, const number&);
  number operator/(const number&, const number&);

  number& operator++(number&);
  number& operator--(number&);

  bool operator==(const number&, const number&);
  bool operator!=(const number&, const number&);
  bool operator<(const number&, const number&);
  bool operator>(const number&, const number&);
  bool operator<=(const number&, const number&);
  bool operator>=(const number&, const number&);

  std::ostream& operator<<(std::ostream&, const number&);
}

#endif /* !LASKIN_NUMBER_HPP_GUARD */
