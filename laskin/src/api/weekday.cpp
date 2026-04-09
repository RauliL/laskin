/*
 * Copyright (c) 2018-2026, Rauli Laine
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
#include "laskin/context.hpp"
#include "laskin/error.hpp"

using namespace laskin;

/**
 * sunday ( -- weekday )
 *
 * Returns Sunday.
 */
LASKIN_BUILTIN_WORD(w_sunday)
{
  context << weekday::sun;
}

/**
 * monday ( -- weekday )
 *
 * Returns Monday.
 */
LASKIN_BUILTIN_WORD(w_monday)
{
  context << weekday::mon;
}

/**
 * tuesday ( -- weekday )
 *
 * Returns Tuesday.
 */
LASKIN_BUILTIN_WORD(w_tuesday)
{
  context << weekday::tue;
}

/**
 * wednesday ( -- weekday )
 *
 * Returns Wednesday.
 */
LASKIN_BUILTIN_WORD(w_wednesday)
{
  context << weekday::wed;
}

/**
 * thursday ( -- weekday )
 *
 * Returns Thursday.
 */
LASKIN_BUILTIN_WORD(w_thursday)
{
  context << weekday::thu;
}

/**
 * friday ( -- weekday )
 *
 * Returns Friday.
 */
LASKIN_BUILTIN_WORD(w_friday)
{
  context << weekday::fri;
}

/**
 * saturday ( -- saturday )
 *
 * Returns Saturday.
 */
LASKIN_BUILTIN_WORD(w_saturday)
{
  context << weekday::sat;
}

/**
 * weekday:weekend? ( weekday -- weekday boolean )
 *
 * Tests whether given day of the week is on weekend.
 */
LASKIN_BUILTIN_WORD(w_is_weekend)
{
  const auto weekday = context.peek().as_weekday();

  context << (weekday == weekday::sat || weekday == weekday::sun);
}

/**
 * weekday:>number ( weekday -- number )
 *
 * Converts day of the week into number where Sunday is 1 and Saturday 7.
 */
LASKIN_BUILTIN_WORD(w_to_number)
{
  const auto value = static_cast<int>(context.pop().as_weekday()) + 1;

  context << value;
}

namespace laskin::api
{
  extern "C" const context::dictionary_definition weekday =
  {
    // Constants.
    { U"sunday", w_sunday },
    { U"monday", w_monday },
    { U"tuesday", w_tuesday },
    { U"wednesday", w_wednesday },
    { U"thursday", w_thursday },
    { U"friday", w_friday },
    { U"saturday", w_saturday },

    // Testing methods.
    { U"weekday:weekend?", w_is_weekend },

    // Conversions.
    { U"weekday:>number", w_to_number }
  };
}
