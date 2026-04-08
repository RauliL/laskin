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
#include "./line-display.hpp"

namespace laskin::gui
{
  LineDisplay::LineDisplay()
    : m_text_buffer(m_text_view.get_buffer())
    , m_input_tag(m_text_buffer->create_tag("input"))
    , m_output_tag(m_text_buffer->create_tag("output"))
    , m_error_tag(m_text_buffer->create_tag("error"))
  {
    m_text_view.set_monospace(true);
    m_text_view.set_editable(false);
    m_text_view.set_wrap_mode(Gtk::WRAP_CHAR);
    m_text_view.signal_size_allocate().connect(
      [this](Gtk::Allocation& allocation)
      {
        const auto adj = m_scrolled_window.get_vadjustment();

        adj->set_value(adj->get_upper() - adj->get_page_size());
      }
    );

    m_scrolled_window.add(m_text_view);
    m_scrolled_window.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);

    add(m_scrolled_window);

    m_input_tag->property_foreground().set_value("gray");
    m_error_tag->property_foreground().set_value("red");
  }

  void
  LineDisplay::clear()
  {
    m_text_buffer->erase(m_text_buffer->begin(), m_text_buffer->end());
  }

  void
  LineDisplay::add_line(const Glib::ustring& line, LineType type)
  {
    Glib::RefPtr<Gtk::TextTag> tag;
    Glib::RefPtr<Gtk::Adjustment> adjustment;
    auto end = m_text_buffer->end();

    switch (type)
    {
    case LINE_TYPE_INPUT:
      tag = m_input_tag;
      break;

    case LINE_TYPE_OUTPUT:
      tag = m_output_tag;
      break;

    case LINE_TYPE_ERROR:
      tag = m_error_tag;
      break;
    }

    m_text_buffer->insert_with_tag(end, line, tag);
  }

  void
  LineDisplay::scroll_down()
  {
    const auto adj = m_scrolled_window.get_vadjustment();
    const auto page = adj->get_page_size();
    const auto value = adj->get_value();
    const auto max = adj->get_upper();

    if (value + page < max)
    {
      adj->set_value(value + page);
    }
    else if (value < max)
    {
      adj->set_value(max);
    }
  }

  void
  LineDisplay::scroll_up()
  {
    const auto adj = m_scrolled_window.get_vadjustment();
    const auto page = adj->get_page_size();
    const auto value = adj->get_value();
    const auto min = adj->get_lower();

    if (value - page > min)
    {
      adj->set_value(value - page);
    }
    else if (value > min)
    {
      adj->set_value(min);
    }
  }
}
