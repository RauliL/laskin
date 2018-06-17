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
#ifndef LASKIN_UNIT_HPP_GUARD
#define LASKIN_UNIT_HPP_GUARD

#include <iostream>
#include <string>

namespace laskin
{
  enum class unit
  {
    // No unit.
    none,

    // Length units.
    mm,
    cm,
    m,
    km,

    // Mass units.
    mg,
    g,
    kg,

    // Time units.
    ms,
    s,
    min,
    h,
    d
  };

  enum class quantity
  {
    none,
    length,
    mass,
    time
  };

  unit unit_base(unit);
  quantity unit_quantity(unit);

  std::u32string unit_name(unit);
  std::u32string quantity_name(quantity);

  unit find_unit_by_name(const std::u32string&);

  std::ostream& operator<<(std::ostream&, unit);
  std::ostream& operator<<(std::ostream&, quantity);
}

#endif /* !LASKIN_UNIT_HPP_GUARD */
