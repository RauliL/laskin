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

#include "laskin/unit.hpp"

namespace laskin
{
  // Supported length units.
  static const unit unit_length_mm = unit(
    unit::type::length,
    U"mm",
    -1000
  );
  static const unit unit_length_cm = unit(
    unit::type::length,
    U"cm",
    -100
  );
  static const unit unit_length_m = unit(
    unit::type::length,
    U"m",
    1
  );
  static const unit unit_length_km = unit(
    unit::type::length,
    U"km",
    1000
  );

  // Supported mass units.
  static const unit unit_mass_mg = unit(
    unit::type::mass,
    U"mg",
    -1000000
  );
  static const unit unit_mass_g = unit(
    unit::type::mass,
    U"g",
    -1000
  );
  static const unit unit_mass_kg = unit(
    unit::type::mass,
    U"kg",
    1
  );

  // Supported time units.
  static const unit unit_time_ms = unit(
    unit::type::time,
    U"ms",
    -1000
  );
  static const unit unit_time_s = unit(
    unit::type::time,
    U"s",
    1
  );
  static const unit unit_time_min = unit(
    unit::type::time,
    U"min",
    60
  );
  static const unit unit_time_h = unit(
    unit::type::time,
    U"h",
    3600
  );
  static const unit unit_time_d = unit(
    unit::type::time,
    U"d",
    86400
  );

  static const std::unordered_map<std::u32string, unit> symbol_mapping =
  {
    // Length units.
    { U"mm", unit_length_mm },
    { U"cm", unit_length_cm },
    { U"m", unit_length_m },
    { U"km", unit_length_km },

    // Mass units.
    { U"mg", unit_mass_mg },
    { U"g", unit_mass_g },
    { U"kg", unit_mass_kg },

    // Time units.
    { U"ms", unit_time_ms },
    { U"s", unit_time_s },
    { U"min", unit_time_min },
    { U"h", unit_time_h },
    { U"d", unit_time_d },
  };

  static const std::vector<unit> all_length_units =
  {
    unit_length_km,
    unit_length_m,
    unit_length_cm,
    unit_length_mm
  };

  static const std::vector<unit> all_mass_units =
  {
    unit_mass_kg,
    unit_mass_g,
    unit_mass_mg
  };

  static const std::vector<unit> all_time_units =
  {
    unit_time_d,
    unit_time_h,
    unit_time_min,
    unit_time_s,
    unit_time_ms
  };

  std::optional<unit> unit::find_by_symbol(const std::u32string& symbol)
  {
    const auto entry = symbol_mapping.find(symbol);

    if (entry != std::end(symbol_mapping))
    {
      return entry->second;
    }

    return std::optional<unit>();
  }

  const unit& unit::base_unit_of(enum type type)
  {
    switch (type)
    {
      case type::length:
        return unit_length_m;

      case type::mass:
        return unit_mass_kg;

      case type::time:
        return unit_time_s;
    }

    // Just to keep the compiler happy.
    return unit_length_m;
  }

  const std::vector<unit>& unit::all_units_of(enum type type)
  {
    switch (type)
    {
      case type::length:
        return all_length_units;

      case type::mass:
        return all_mass_units;

      case type::time:
        return all_time_units;
    }

    // Just to keep the compiler happy.
    return all_length_units;
  }

  unit::unit(enum type type, const std::u32string& symbol, int multiplier)
    : m_type(type)
    , m_symbol(symbol)
    , m_multiplier(multiplier) {}

  unit::unit(const unit& that)
    : m_type(that.m_type)
    , m_symbol(that.m_symbol)
    , m_multiplier(that.m_multiplier) {}

  unit& unit::operator=(const unit& that)
  {
    m_type = that.m_type;
    m_symbol = that.m_symbol;
    m_multiplier = that.m_multiplier;

    return *this;
  }

  std::u32string to_string(enum unit::type type)
  {
    switch (type)
    {
      case unit::type::length:
        return U"length";

      case unit::type::mass:
        return U"mass";

      case unit::type::time:
        return U"time";
    }

    return U"unknown";
  }
}
