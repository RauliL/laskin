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
#ifndef LASKIN_VALUE_HPP_GUARD
#define LASKIN_VALUE_HPP_GUARD

#include <iostream>
#include <string>
#include <vector>

#include <gmpxx.h>

namespace laskin
{
  class quote;

  /**
   * Wrapper class for supported value types.
   */
  class value
  {
  public:
    /**
     * Enumeration of different supported value types.
     */
    enum type
    {
      type_boolean,
      type_number,
      type_quote,
      type_vector
    };

    /**
     * Constructs boolean value.
     */
    static value make_boolean(bool value);

    /**
     * Constructs number value.
     */
    static value make_number(const mpf_class& value);

    /**
     * Constructs vector value.
     */
    static value make_vector(const std::vector<value>& elements);

    /**
     * Constructs quote value.
     */
    static value make_quote(const class quote& quote);

    /**
     * Constructs boolean value of false.
     */
    explicit value();

    /**
     * Constructs copy of existing value.
     */
    value(const value& that);

    /**
     * Move constructor which transfers contents from another value into this
     * one.
     */
    value(value&& that);

    /**
     * Destructor.
     */
    ~value();

    /**
     * Copies contents of another value into this one.
     */
    value& operator=(const value& that);

    /**
     * Moves contents of another value into this one.
     */
    value& operator=(value&& that);

    /**
     * Returns type of the value.
     */
    inline enum type type() const
    {
      return m_type;
    }

    /**
     * Tests whether the value is of given type.
     */
    inline bool is(enum type type) const
    {
      return m_type == type;
    }

    /**
     * Returns textual description of a value type.
     */
    static std::u32string type_description(enum type type);

    /**
     * Resets the value back to it's initial state.
     */
    void reset();

    bool as_boolean() const;
    const mpf_class& as_number() const;
    const std::vector<value>& as_vector() const;
    const quote& as_quote() const;

    /**
     * Constructs string representation of the value.
     */
    std::u32string to_string() const;

    /**
     * Constructs the most appropriate representation of what the value would
     * look like in source code.
     */
    std::u32string to_source() const;

    /**
     * Compares two values against each other.
     */
    int compare(const value& that) const;

    inline bool operator==(const value& that) const
    {
      return compare(that) == 0;
    }

    inline bool operator!=(const value& that) const
    {
      return compare(that) != 0;
    }

    inline bool operator<(const value& that) const
    {
      return compare(that) < 0;
    }

    inline bool operator>(const value& that) const
    {
      return compare(that) > 0;
    }

    inline bool operator<=(const value& that) const
    {
      return compare(that) <= 0;
    }

    inline bool operator>=(const value& that) const
    {
      return compare(that) >= 0;
    }

  private:
    /** Type of the value. */
    enum type m_type;
    union
    {
      bool m_value_boolean;
      mpf_class* m_value_number;
      std::vector<value>* m_value_vector;
      quote* m_value_quote;
    };
  };

  std::ostream& operator<<(std::ostream&, enum value::type);
  std::ostream& operator<<(std::ostream&, const value&);
}

#endif /* !LASKIN_VALUE_HPP_GUARD */
