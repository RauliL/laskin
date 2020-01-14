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
#include <chrono>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"

namespace laskin
{
  static void w_now(class context& context, std::ostream&)
  {
    const auto now = std::chrono::system_clock::now();
    const auto ts = std::chrono::system_clock::to_time_t(now);
    const auto tm = std::localtime(&ts);

    try
    {
      context << value::make_time(peelo::chrono::time(
        tm->tm_hour,
        tm->tm_min,
        tm->tm_sec
      ));
    }
    catch (const std::invalid_argument&)
    {
      throw error(
        error::type::system,
        U"System clock returned invalid time."
      );
    }
  }

  static void w_hour(class context& context, std::ostream&)
  {
    context << value::make_number(context.peek().as_time().hour());
  }

  static void w_minute(class context& context, std::ostream&)
  {
    context << value::make_number(context.peek().as_time().minute());
  }

  static void w_second(class context& context, std::ostream&)
  {
    context << value::make_number(context.peek().as_time().second());
  }

  static void w_format(class context& context, std::ostream&)
  {
    using peelo::unicode::encoding::utf8::encode;
    using peelo::unicode::encoding::utf8::decode;
    const auto time = context.pop().as_time();
    const auto format = context.pop().as_string();

    context << value::make_string(decode(time.format(encode(format))));
  }

  static void w_to_vector(class context& context, std::ostream&)
  {
    const auto time = context.pop().as_time();

    context << value::make_vector({
      value::make_number(time.hour()),
      value::make_number(time.minute()),
      value::make_number(time.second())
    });
  }

  namespace api
  {
    extern "C" const context::dictionary_definition time_api =
    {
      { U"now", w_now },

      { U"time:hour", w_hour },
      { U"time:minute", w_minute },
      { U"time:second", w_second },

      { U"time:format", w_format },

      { U"time:>vector", w_to_vector }
    };
  }
}
