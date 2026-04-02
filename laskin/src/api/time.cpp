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
#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/macros.hpp"

using namespace laskin;

/**
 * now ( -- time )
 *
 * Returns current time.
 */
BUILTIN_WORD(w_now)
{
  try
  {
    context << value::make_time(peelo::chrono::time::now());
  }
  catch (const std::runtime_error&)
  {
    throw error(
      error::type::system,
      U"System clock returned invalid time."
    );
  }
}

/**
 * time:hour ( time -- time number )
 *
 * Returns hour of the time.
 */
BUILTIN_WORD(w_hour)
{
  context << context.peek().as_time().hour();
}

/**
 * time:minute ( time -- time number )
 *
 * Returns minute of the time.
 */
BUILTIN_WORD(w_minute)
{
  context << context.peek().as_time().minute();
}

/**
 * time:second ( time -- time number )
 *
 * Returns seconds of the time.
 */
BUILTIN_WORD(w_second)
{
  context << context.peek().as_time().second();
}

/**
 * time:format ( string time -- string )
 *
 * Formats time with strftime().
 */
BUILTIN_WORD(w_format)
{
  using peelo::unicode::encoding::utf8::encode;
  using peelo::unicode::encoding::utf8::decode;

  const auto time = context.pop().as_time();
  const auto format = context.pop().as_string();

  context << decode(time.format(encode(format)));
}

/**
 * time:>number ( time -- number )
 *
 * Constructs UNIX timestamp from the time.
 */
BUILTIN_WORD(w_to_number)
{
  const auto time = context.pop().as_time();
  number result(0L, unit::second);

  result += time.hour() * peelo::chrono::duration::seconds_per_hour;
  result += time.minute() * peelo::chrono::duration::seconds_per_minute;
  result += time.second();

  context << result;
}

/**
 * time:>vector ( time -- vector )
 *
 * Extracts hour, minutes and seconds from the time and returns them as a
 * vector of number.
 */
BUILTIN_WORD(w_to_vector)
{
  const auto time = context.pop().as_time();

  context << value::make_vector({
    value::make_number(time.hour()),
    value::make_number(time.minute()),
    value::make_number(time.second())
  });
}

namespace laskin::api
{
  extern "C" const context::dictionary_definition time_api =
  {
    { U"now", w_now },

    { U"time:hour", w_hour },
    { U"time:minute", w_minute },
    { U"time:second", w_second },

    { U"time:format", w_format },

    { U"time:>number", w_to_number },
    { U"time:>vector", w_to_vector }
  };
}
