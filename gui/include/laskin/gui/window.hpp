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

#include "laskin/gui/context.hpp"
#include "laskin/gui/dictionary-display.hpp"
#include "laskin/gui/line-display.hpp"
#include "laskin/gui/line-editor.hpp"
#include "laskin/gui/stack-display.hpp"

namespace laskin::gui
{
  class Window : public Gtk::Window
  {
  public:
    static const int DEFAULT_WIDTH;
    static const int DEFAULT_HEIGHT;

    explicit Window(const Glib::RefPtr<Context>& context);

  protected:
    void on_show();
    void on_line_received(const Glib::ustring& line);
    void on_error_thrown(const laskin::error& error);
    void on_text_written(const Glib::ustring& text);
    bool on_key_press_event(GdkEventKey* event);
    void on_word_activated(
      const Glib::ustring& id,
      const Glib::ustring& value_source
    );

  private:
    Glib::RefPtr<Context> m_context;
    Glib::ustring m_source;
    std::stack<char32_t> m_open_braces;
    Gtk::Box m_box;
    Gtk::Paned m_paned;
    LineDisplay m_line_display;
    Gtk::Notebook m_notebook;
    StackDisplay m_stack_display;
    DictionaryDisplay m_dictionary_display;
    LineEditor m_line_editor;
  };
}
