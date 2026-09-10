/*
 * Copyright (c) 2023-2026, Rauli Laine
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

#include <cstddef>
#include <vector>

#include <gtkmm.h>
#include <sigc++/signal.h>

#include "./syntax-highlighter.hpp"

namespace laskin::gui
{
  class LineEditor : public Gtk::Box
  {
  public:
    using line_received_signal = sigc::signal<void(const Glib::ustring&)>;

    explicit LineEditor();

    void grab_focus();

    inline int get_line_count() const
    {
      return m_line_count;
    }

    void set_line_count(int line_count);

    inline int get_stack_depth_count() const
    {
      return m_stack_depth_count;
    }

    void set_stack_depth_count(int stack_depth_count);

    void set_text(const Glib::ustring& text);

    inline line_received_signal& signal_line_received()
    {
      return m_signal_line_received;
    }

    inline const line_received_signal& signal_line_received() const
    {
      return m_signal_line_received;
    }

  protected:
    void on_activate();

    void update_prompt();

    void on_buffer_changed();

    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

    Glib::ustring get_text() const;

    void add_to_history(const Glib::ustring& line);

    void history_previous();

    void history_next();

    void place_cursor_at_end();

    void reset_history_navigation();

  private:
    static constexpr std::size_t HISTORY_MAX_LEN = 100;

    int m_line_count;
    int m_stack_depth_count;
    bool m_updating;
    std::vector<Glib::ustring> m_history;
    std::size_t m_history_index;
    Glib::ustring m_history_draft;
    Gtk::Label m_label;
    Gtk::TextView m_text_view;
    Glib::RefPtr<Gtk::TextBuffer> m_text_buffer;
    Glib::RefPtr<Gtk::EventControllerKey> m_key_controller;
    SyntaxHighlighter m_highlighter;
    line_received_signal m_signal_line_received;
  };
}
