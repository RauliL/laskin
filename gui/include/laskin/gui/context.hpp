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
#pragma once

#include "laskin/context.hpp"
#include "laskin/error.hpp"

#include <glibmm.h>

namespace laskin::gui
{
  class Context : public Glib::ObjectBase
  {
  public:
    using error_thrown_signal = sigc::signal<void, laskin::error>;
    using text_written_signal = sigc::signal<void, Glib::ustring>;

    explicit Context();

    inline context::container_type& stack()
    {
      return m_context.data();
    }

    inline const context::container_type& stack() const
    {
      return m_context.data();
    }

    inline context::dictionary_type& dictionary()
    {
      return m_context.dictionary();
    }

    inline const context::dictionary_type& dictionary() const
    {
      return m_context.dictionary();
    }

    void execute(const Glib::ustring& source_code, int line = 1);

    inline error_thrown_signal& signal_error_thrown()
    {
      return m_signal_error_thrown;
    }

    inline const error_thrown_signal& signal_error_thrown() const
    {
      return m_signal_error_thrown;
    }

    inline text_written_signal& signal_text_written()
    {
      return m_signal_text_written;
    }

    inline const text_written_signal& signal_text_written() const
    {
      return m_signal_text_written;
    }

  private:
    laskin::context m_context;
    error_thrown_signal m_signal_error_thrown;
    text_written_signal m_signal_text_written;
  };
}
