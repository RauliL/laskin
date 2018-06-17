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
#include <unordered_map>

#include <peelo/unicode.hpp>

#include "laskin/unit.hpp"

namespace laskin
{
  static const std::unordered_map<unit, quantity> unit_quantity_mapping =
  {
    // Length units.
    { unit::mm, quantity::length },
    { unit::cm, quantity::length },
    { unit::m, quantity::length },
    { unit::km, quantity::length },

    // Mass units.
    { unit::mg, quantity::mass },
    { unit::g, quantity::mass },
    { unit::kg, quantity::mass },

    // Time units.
    { unit::ms, quantity::time },
    { unit::s, quantity::time },
    { unit::min, quantity::time },
    { unit::h, quantity::time },
    { unit::d, quantity::time }
  };

  static const std::unordered_map<unit, unit> unit_base_mapping =
  {
    // Length units.
    { unit::mm, unit::m },
    { unit::cm, unit::m },
    { unit::km, unit::m },

    // Mass units.
    { unit::mg, unit::kg },
    { unit::g, unit::kg },

    // Time units.
    { unit::ms, unit::s },
    { unit::min, unit::s },
    { unit::h, unit::s },
    { unit::d, unit::s }
  };

  static const std::unordered_map<unit, std::u32string> unit_name_mapping =
  {
    // Length units.
    { unit::mm, U"mm" },
    { unit::cm, U"cm" },
    { unit::m, U"m" },
    { unit::km, U"km" },

    // Mass units.
    { unit::mg, U"mg" },
    { unit::g, U"g" },
    { unit::kg, U"kg" },

    // Time units.
    { unit::ms, U"ms" },
    { unit::s, U"s" },
    { unit::min, U"min" },
    { unit::h, U"h" },
    { unit::d, U"d" }
  };

  quantity unit_quantity(unit u)
  {
    const auto entry = unit_quantity_mapping.find(u);

    if (entry != std::end(unit_quantity_mapping))
    {
      return entry->second;
    } else {
      return quantity::none;
    }
  }

  unit unit_base(unit u)
  {
    const auto entry = unit_base_mapping.find(u);

    return entry == std::end(unit_base_mapping) ? u : entry->second;
  }

  unit find_unit_by_name(const std::u32string& input)
  {
    for (const auto& mapping : unit_name_mapping)
    {
      if (!mapping.second.compare(input))
      {
        return mapping.first;
      }
    }

    return unit::none;
  }

  std::u32string unit_name(unit u)
  {
    const auto entry = unit_name_mapping.find(u);

    if (entry != std::end(unit_name_mapping))
    {
      return entry->second;
    } else {
      return U"";
    }
  }

  std::u32string quantity_name(quantity q)
  {
    switch (q)
    {
      case quantity::none:
        return U"no unit";

      case quantity::length:
        return U"length";

      case quantity::mass:
        return U"mass";

      case quantity::time:
        return U"time";
    }

    return U"";
  }

  std::ostream& operator<<(std::ostream& out, unit u)
  {
    out << peelo::unicode::utf8::encode(unit_name(u));

    return out;
  }

  std::ostream& operator<<(std::ostream& out, quantity q)
  {
    out << peelo::unicode::utf8::encode(quantity_name(q));

    return out;
  }
}
