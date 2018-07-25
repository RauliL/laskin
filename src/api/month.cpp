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
#include "laskin/context.hpp"

namespace laskin
{
  static void w_january(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::jan);
  }

  static void w_february(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::feb);
  }

  static void w_march(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::mar);
  }

  static void w_april(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::apr);
  }

  static void w_may(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::may);
  }

  static void w_june(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::jun);
  }

  static void w_july(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::jul);
  }

  static void w_august(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::aug);
  }

  static void w_september(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::sep);
  }

  static void w_october(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::oct);
  }

  static void w_november(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::nov);
  }

  static void w_december(class context& context, std::ostream&)
  {
    context << value::make_month(peelo::month::dec);
  }

  static void w_to_number(class context& context, std::ostream&)
  {
    const auto value = static_cast<int>(context.pop().as_month()) + 1;

    context << value::make_number(value);
  }

  namespace api
  {
    extern "C" const context::dictionary_definition month =
    {
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

      { U"month:>number", w_to_number }
    };
  }
}
