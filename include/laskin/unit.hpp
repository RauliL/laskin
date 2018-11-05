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

#include <optional>
#include <string>
#include <vector>

namespace laskin
{
  /**
   * Representation of measurement unit.
   */
  class unit
  {
  public:
    /**
     * Enumeration of different supported types of measurement.
     */
    enum class type
    {
      length,
      mass,
      time
    };

    static std::optional<unit> find_by_symbol(const std::u32string& symbol);

    static const unit& base_unit_of(enum type type);

    static const std::vector<unit>& all_units_of(enum type type);

    explicit unit(
      enum type type,
      const std::u32string& symbol,
      int multiplier
    );

    /**
     * Constructs copy of existing measurement unit.
     */
    unit(const unit& that);

    unit& operator=(const unit& that);

    inline enum type type() const
    {
      return m_type;
    }

    inline const std::u32string& symbol() const
    {
      return m_symbol;
    }

    inline int multiplier() const
    {
      return m_multiplier;
    }

    inline bool is_base_unit() const
    {
      return m_multiplier == 1;
    }

  private:
    enum type m_type;
    std::u32string m_symbol;
    int m_multiplier;
  };

  std::u32string to_string(enum unit::type);

#if 0
  namespace unit
  {
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
#endif
}

#endif /* !LASKIN_UNIT_HPP_GUARD */
