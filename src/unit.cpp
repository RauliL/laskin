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
  namespace unit
  {
    optional_any base_unit_of(const optional_any& unit)
    {
      if (unit)
      {
        return base_unit_of(unit.value());
      } else {
        return optional_any();
      }
    }

    any base_unit_of(const any& unit)
    {
      struct visitor
      {
        any operator()(length) const
        {
          return length::m;
        }

        any operator()(mass) const
        {
          return mass::kg;
        }

        any operator()(time) const
        {
          return time::s;
        }
      };

      return std::visit(visitor(), unit);
    }

    static const std::unordered_map<std::u32string, any> name_mapping =
    {
      // Length units.
      { U"mm", length::mm },
      { U"cm", length::cm },
      { U"m", length::m },
      { U"km", length::km },

      // Mass units.
      { U"mg", mass::mg },
      { U"g", mass::g },
      { U"kg", mass::kg },

      // Time units.
      { U"ms", time::ms },
      { U"s", time::s },
      { U"min", time::min },
      { U"h", time::h },
      { U"d", time::d }
    };

    optional_any find_by_name(const std::u32string& name)
    {
      const auto entry = name_mapping.find(name);

      if (entry != std::end(name_mapping))
      {
        return entry->second;
      }

      return optional_any();
    }

    std::u32string name_of(const any& unit)
    {
      struct visitor
      {
        std::u32string operator()(length x) const
        {
          return name_of(x);
        }

        std::u32string operator()(mass x) const
        {
          return name_of(x);
        }

        std::u32string operator()(time x) const
        {
          return name_of(x);
        }
      };

      return std::visit(visitor(), unit);
    }

    std::u32string name_of(length u)
    {
      switch (u)
      {
        case length::mm:
          return U"mm";

        case length::cm:
          return U"cm";

        case length::m:
          return U"m";

        case length::km:
          return U"km";
      }

      return U"unknown";
    }

    std::u32string name_of(mass u)
    {
      switch (u)
      {
        case mass::mg:
          return U"mg";

        case mass::g:
          return U"g";

        case mass::kg:
          return U"kg";
      }

      return U"unknown";
    }

    std::u32string name_of(time u)
    {
      switch (u)
      {
        case time::ms:
          return U"ms";

        case time::s:
          return U"s";

        case time::min:
          return U"min";

        case time::h:
          return U"h";

        case time::d:
          return U"d";
      }

      return U"unknown";
    }

    std::u32string quantity_of(const optional_any& unit)
    {
      if (unit)
      {
        return quantity_of(unit.value());
      } else {
        return U"no unit";
      }
    }

    std::u32string quantity_of(const any& unit)
    {
      struct visitor
      {
        std::u32string operator()(length) const
        {
          return U"length";
        }

        std::u32string operator()(mass) const
        {
          return U"mass";
        }

        std::u32string operator()(time) const
        {
          return U"time";
        }
      };

      return std::visit(visitor(), unit);
    }
  }
}
