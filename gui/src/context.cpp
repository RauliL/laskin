/*
 * Copyright (c) 2023, Rauli Laine
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
#include <sstream>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/gui/utils.hpp"
#include "laskin/gui/window.hpp"

namespace laskin::gui
{
  Context::Context() {}

  void Context::execute(const Glib::ustring& source_code, int line)
  {
    std::stringstream buffer;

    try
    {
      quote::parse(
        utils::string_convert<std::u32string, Glib::ustring>(source_code),
        line
      ).call(m_context, buffer);
    }
    catch (const error& e)
    {
      m_signal_error_thrown.emit(e);
      return;
    }

    const auto output = buffer.str();

    if (output.length() > 0)
    {
      m_signal_text_written.emit(
        utils::string_convert<Glib::ustring, std::u32string>(
          peelo::unicode::encoding::utf8::decode(output)
        )
      );
    }
  }
}
