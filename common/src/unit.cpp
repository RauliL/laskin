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
  const unit unit::millimeter(
    unit::type::length,
    U"mm",
    -1000
  );
  const unit unit::centimeter(
    unit::type::length,
    U"cm",
    -100
  );
  const unit unit::meter(
    unit::type::length,
    U"m",
    1
  );
  const unit unit::kilometer(
    unit::type::length,
    U"km",
    1000
  );

  // Supported mass units.
  const unit unit::milligram(
    unit::type::mass,
    U"mg",
    -1000000
  );
  const unit unit::gram(
    unit::type::mass,
    U"g",
    -1000
  );
  const unit unit::kilogram(
    unit::type::mass,
    U"kg",
    1
  );

  // Supported time units.
  const unit unit::millisecond(
    unit::type::time,
    U"ms",
    -1000
  );
  const unit unit::second(
    unit::type::time,
    U"s",
    1
  );
  const unit unit::minute(
    unit::type::time,
    U"min",
    60
  );
  const unit unit::hour(
    unit::type::time,
    U"h",
    3600
  );
  const unit unit::day(
    unit::type::time,
    U"d",
    86400
  );

  static const std::unordered_map<std::u32string, unit> symbol_mapping =
  {
    // Length units.
    { U"mm", unit::millimeter },
    { U"cm", unit::centimeter },
    { U"m", unit::meter },
    { U"km", unit::kilometer },

    // Mass units.
    { U"mg", unit::milligram },
    { U"g", unit::gram },
    { U"kg", unit::kilogram },

    // Time units.
    { U"ms", unit::millisecond },
    { U"s", unit::second },
    { U"min", unit::minute },
    { U"h", unit::hour },
    { U"d", unit::day },
  };

  static const std::vector<unit> all_length_units =
  {
    unit::kilometer,
    unit::meter,
    unit::centimeter,
    unit::millimeter
  };

  static const std::vector<unit> all_mass_units =
  {
    unit::kilogram,
    unit::gram,
    unit::milligram
  };

  static const std::vector<unit> all_time_units =
  {
    unit::day,
    unit::hour,
    unit::minute,
    unit::second,
    unit::millisecond
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
        return unit::meter;

      case type::mass:
        return unit::kilogram;

      case type::time:
        return unit::second;
    }

    // Just to keep the compiler happy.
    return unit::meter;
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
