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
#include "laskin/macros.hpp"

using namespace laskin;

/**
 * january ( -- month )
 *
 * Returns January.
 */
BUILTIN_WORD(w_january)
{
  context << value::make_month(peelo::chrono::month::jan);
}

/**
 * february ( -- month )
 *
 * Returns February.
 */
BUILTIN_WORD(w_february)
{
  context << value::make_month(peelo::chrono::month::feb);
}

/**
 * march ( -- month )
 *
 * Return March.
 */
BUILTIN_WORD(w_march)
{
  context << value::make_month(peelo::chrono::month::mar);
}

/**
 * april ( -- month )
 *
 * Returns April.
 */
BUILTIN_WORD(w_april)
{
  context << value::make_month(peelo::chrono::month::apr);
}

/**
 * may ( -- month )
 *
 * Returns May.
 */
BUILTIN_WORD(w_may)
{
  context << value::make_month(peelo::chrono::month::may);
}

/**
 * june ( -- month )
 *
 * Returns June.
 */
BUILTIN_WORD(w_june)
{
  context << value::make_month(peelo::chrono::month::jun);
}

/**
 * july ( -- month )
 *
 * Returns July.
 */
BUILTIN_WORD(w_july)
{
  context << value::make_month(peelo::chrono::month::jul);
}

/**
 * august ( -- month )
 *
 * Returns August.
 */
BUILTIN_WORD(w_august)
{
  context << value::make_month(peelo::chrono::month::aug);
}

/**
 * september ( -- month )
 *
 * Returns September.
 */
BUILTIN_WORD(w_september)
{
  context << value::make_month(peelo::chrono::month::sep);
}

/**
 * october ( -- month )
 *
 * Returns October.
 */
BUILTIN_WORD(w_october)
{
  context << value::make_month(peelo::chrono::month::oct);
}

/**
 * november ( -- month )
 *
 * Returns November.
 */
BUILTIN_WORD(w_november)
{
  context << value::make_month(peelo::chrono::month::nov);
}

/**
 * december ( -- month )
 *
 * Returns December.
 */
BUILTIN_WORD(w_december)
{
  context << value::make_month(peelo::chrono::month::dec);
}

/**
 * month:>number ( month -- number )
 *
 * Converts month into a number. January is 1, December 12 and so on.
 */
BUILTIN_WORD(w_to_number)
{
  const auto value = static_cast<int>(context.pop().as_month()) + 1;

  context << value;
}

namespace api
{
  extern "C" const context::dictionary_definition month =
  {
    // Constants.
    { U"january", w_january },
    { U"february", w_february },
    { U"march", w_march },
    { U"april", w_april },
    { U"may", w_may },
    { U"june", w_june },
    { U"july", w_july },
    { U"august", w_august },
    { U"september", w_september },
    { U"october", w_october },
    { U"november", w_november },
    { U"december", w_december },

    // Conversions.
    { U"month:>number", w_to_number }
  };
}
