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
#include "./line-editor.hpp"
#include "./utils.hpp"

namespace laskin::gui
{
  LineEditor::LineEditor()
    : Gtk::Box(Gtk::Orientation::HORIZONTAL)
    , m_line_count(1)
    , m_stack_depth_count(0)
    , m_updating(false)
    , m_history_index(0)
    , m_text_buffer(m_text_view.get_buffer())
    , m_key_controller(Gtk::EventControllerKey::create())
    , m_highlighter(m_text_buffer)
  {
    update_prompt();

    m_text_view.set_editable(true);
    m_text_view.set_accepts_tab(false);
    m_text_view.set_cursor_visible(true);
    m_text_view.set_wrap_mode(Gtk::WrapMode::NONE);
    m_text_view.set_hexpand(true);
    m_text_view.set_vexpand(false);
    m_text_view.set_left_margin(0);
    m_text_view.set_right_margin(0);
    m_text_view.set_top_margin(2);
    m_text_view.set_bottom_margin(2);
    utils::set_monospace_font(m_text_view);
    utils::set_monospace_font(m_label);

    append(m_label);
    append(m_text_view);

    m_text_buffer->signal_changed().connect(sigc::mem_fun(
      *this,
      &LineEditor::on_buffer_changed
    ));
    m_key_controller->signal_key_pressed().connect(
      sigc::mem_fun(*this, &LineEditor::on_key_pressed),
      false
    );
    m_text_view.add_controller(m_key_controller);
  }

  void
  LineEditor::grab_focus()
  {
    m_text_view.grab_focus();
  }

  void
  LineEditor::set_line_count(int line_count)
  {
    if (m_line_count != line_count)
    {
      m_line_count = line_count;
      update_prompt();
    }
  }

  void
  LineEditor::set_stack_depth_count(int stack_depth_count)
  {
    if (m_stack_depth_count != stack_depth_count)
    {
      m_stack_depth_count = stack_depth_count;
      update_prompt();
    }
  }

  void
  LineEditor::set_text(const Glib::ustring& text)
  {
    reset_history_navigation();

    m_updating = true;
    m_text_buffer->set_text(text);
    m_updating = false;

    auto line_start = m_text_buffer->begin();

    m_highlighter.highlight_line(line_start);
    place_cursor_at_end();
  }

  Glib::ustring
  LineEditor::get_text() const
  {
    auto text = m_text_buffer->get_text();

    while (!text.empty() && text[text.length() - 1] == U'\n')
    {
      text.erase(text.length() - 1);
    }

    return text;
  }

  void
  LineEditor::on_activate()
  {
    const Glib::ustring line = get_text();

    set_line_count(m_line_count + 1);
    add_to_history(line);
    m_signal_line_received.emit(line);

    m_updating = true;
    m_text_buffer->set_text(Glib::ustring());
    m_updating = false;
  }

  void
  LineEditor::on_buffer_changed()
  {
    if (m_updating)
    {
      return;
    }

    const auto newline = get_text().find('\n');

    if (newline != Glib::ustring::npos)
    {
      const Glib::ustring line = get_text().substr(0, newline);

      m_updating = true;
      m_text_buffer->set_text(line);
      m_updating = false;
      on_activate();

      return;
    }

    auto line_start = m_text_buffer->get_insert()->get_iter();

    line_start.set_line_offset(0);
    m_highlighter.highlight_line(line_start);
  }

  void
  LineEditor::add_to_history(const Glib::ustring& line)
  {
    if (line.empty())
    {
      reset_history_navigation();
      return;
    }

    if (!m_history.empty() && m_history.back() == line)
    {
      reset_history_navigation();
      return;
    }

    m_history.push_back(line);

    if (m_history.size() > HISTORY_MAX_LEN)
    {
      m_history.erase(m_history.begin());
    }

    reset_history_navigation();
  }

  void
  LineEditor::reset_history_navigation()
  {
    m_history_index = m_history.size();
    m_history_draft.clear();
  }

  void
  LineEditor::place_cursor_at_end()
  {
    m_text_buffer->place_cursor(m_text_buffer->end());
  }

  void
  LineEditor::history_previous()
  {
    if (m_history.empty())
    {
      return;
    }

    if (m_history_index == m_history.size())
    {
      m_history_draft = get_text();
      m_history_index = m_history.size() - 1;
    } else if (m_history_index > 0)
    {
      --m_history_index;
    } else {
      return;
    }

    m_updating = true;
    m_text_buffer->set_text(m_history[m_history_index]);
    m_updating = false;

    auto line_start = m_text_buffer->begin();

    m_highlighter.highlight_line(line_start);
    place_cursor_at_end();
  }

  void
  LineEditor::history_next()
  {
    if (m_history.empty() || m_history_index >= m_history.size())
    {
      return;
    }

    ++m_history_index;

    m_updating = true;

    if (m_history_index >= m_history.size())
    {
      m_text_buffer->set_text(m_history_draft);
    } else {
      m_text_buffer->set_text(m_history[m_history_index]);
    }

    m_updating = false;

    auto line_start = m_text_buffer->begin();

    m_highlighter.highlight_line(line_start);
    place_cursor_at_end();
  }

  bool
  LineEditor::on_key_pressed(
    guint keyval,
    guint /* keycode */,
    Gdk::ModifierType state
  )
  {
    if (keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter)
    {
      on_activate();

      return true;
    }

    if (
      (state & Gdk::ModifierType::CONTROL_MASK) == Gdk::ModifierType{}
      && (state & Gdk::ModifierType::ALT_MASK) == Gdk::ModifierType{}
    )
    {
      if (keyval == GDK_KEY_Up)
      {
        history_previous();

        return true;
      }

      if (keyval == GDK_KEY_Down)
      {
        history_next();

        return true;
      }
    }

    return false;
  }

  void
  LineEditor::update_prompt()
  {
    m_label.set_text(Glib::ustring::format(
      "laskin:",
      m_line_count,
      ':',
      m_stack_depth_count,
      '>'
    ));
  }
}
