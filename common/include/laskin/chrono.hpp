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
#pragma once

#include <peelo/chrono/date.hpp>
#include <peelo/chrono/month.hpp>
#include <peelo/chrono/time.hpp>
#include <peelo/chrono/weekday.hpp>

namespace laskin
{
  /**
   * Tests whether given string contains valid ISO 8601 date.
   */
  bool is_date(const std::u32string&);

  /**
   * Tests whether given string contains valid ISO 8601 time.
   */
  bool is_time(const std::u32string&);

  /**
   * Tests whether given string is name of an month.
   */
  bool is_month(const std::u32string&);

  /**
   * Tests whether given string is name of an weekday.
   */
  bool is_weekday(const std::u32string&);

  /**
   * Parses given string into a date. The date is expected to be in ISO 8601
   * format.
   */
  peelo::chrono::date parse_date(const std::u32string&);

  /**
   * Parses given string into a time. The time is expected to be in ISO 8601
   * format.
   */
  peelo::chrono::time parse_time(const std::u32string&);

  /**
   * Parses given string into a month.
   */
  peelo::chrono::month parse_month(const std::u32string&);

  /**
   * Parses given string into day of an week.
   */
  peelo::chrono::weekday parse_weekday(const std::u32string&);
}
