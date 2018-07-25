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
#include <optional>
#include <string>
#include <variant>

namespace laskin
{
  namespace unit
  {
    enum class length
    {
      mm,
      cm,
      m,
      km
    };

    enum class mass
    {
      mg,
      g,
      kg
    };

    enum class time
    {
      ms,
      s,
      min,
      h,
      d
    };

    using any = std::variant<length, mass, time>;
    using optional_any = std::optional<any>;

    optional_any base_unit_of(const optional_any&);
    any base_unit_of(const any&);

    optional_any find_by_name(const std::u32string&);

    std::u32string name_of(const any&);
    std::u32string name_of(length);
    std::u32string name_of(mass);
    std::u32string name_of(time);

    std::u32string quantity_of(const optional_any&);
    std::u32string quantity_of(const any&);
  }
}

#endif /* !LASKIN_UNIT_HPP_GUARD */
