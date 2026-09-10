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
#include <sstream>

#include "./utils.hpp"
#include "./window.hpp"

namespace laskin::gui
{
  const int Window::DEFAULT_WIDTH = 640;
  const int Window::DEFAULT_HEIGHT = 480;

  Window::Window(const Glib::RefPtr<Context>& context)
    : m_context(context)
    , m_box(Gtk::Orientation::VERTICAL)
    , m_paned(Gtk::Orientation::HORIZONTAL)
    , m_key_controller(Gtk::EventControllerKey::create())
  {
    set_title("Laskin");
    set_margin(5);
    set_default_size(DEFAULT_WIDTH, DEFAULT_HEIGHT);

    m_line_display.set_vexpand(true);
    m_paned.set_start_child(m_line_display);
    m_paned.set_resize_start_child(true);
    m_paned.set_shrink_start_child(false);
    m_paned.set_end_child(m_notebook);
    m_paned.set_resize_end_child(false);
    m_paned.set_shrink_end_child(false);
    m_paned.set_vexpand(true);

    m_notebook.append_page(m_stack_display, "Stack");
    m_notebook.append_page(m_dictionary_display, "Dictionary");

    m_box.append(m_paned);
    m_box.append(m_line_editor);

    set_child(m_box);

    m_dictionary_display.update(m_context->dictionary());

    m_line_editor.signal_line_received().connect(sigc::mem_fun(
      *this,
      &Window::on_line_received
    ));
    m_context->signal_error_thrown().connect(sigc::mem_fun(
      *this,
      &Window::on_error_thrown
    ));
    m_context->signal_text_written().connect(sigc::mem_fun(
      *this,
      &Window::on_text_written
    ));
    m_dictionary_display.signal_word_activated().connect(sigc::mem_fun(
      *this,
      &Window::on_word_activated
    ));
    m_key_controller->signal_key_pressed().connect(
      sigc::mem_fun(*this, &Window::on_key_pressed),
      false
    );
    add_controller(m_key_controller);
  }

  void
  Window::on_show()
  {
    Gtk::Window::on_show();
    m_line_editor.grab_focus();
  }

  void
  Window::on_line_received(const Glib::ustring& line)
  {
    if (line.empty())
    {
      return;
    }
    m_line_display.add_line(line + '\n', LineDisplay::LINE_TYPE_INPUT);
    m_source.append(line);
    m_source.append(1, '\n');
    utils::count_open_braces(m_open_braces, line);
    if (m_open_braces.empty())
    {
      const auto& stack = m_context->stack();

      m_context->execute(
        m_source,
        m_line_editor.get_line_count() - 1
      );
      m_source.clear();
      m_line_editor.set_stack_depth_count(stack.size());
      m_stack_display.update(stack);
      m_dictionary_display.update(m_context->dictionary());
    }
  }

  void
  Window::on_error_thrown(const laskin::error& error)
  {
    if (error.is(laskin::error::type::exit))
    {
      std::exit(EXIT_SUCCESS);
    } else {
      std::stringstream buffer;

      buffer << error;
      m_line_display.add_line(buffer.str() + '\n', LineDisplay::LINE_TYPE_ERROR);
    }
  }

  void
  Window::on_text_written(const Glib::ustring& text)
  {
    m_line_display.add_line(text, LineDisplay::LINE_TYPE_OUTPUT);
  }

  bool
  Window::on_key_pressed(
    guint keyval,
    guint keycode,
    Gdk::ModifierType state
  )
  {
    if (
      (state & Gdk::ModifierType::CONTROL_MASK) ==
      Gdk::ModifierType::CONTROL_MASK
    )
    {
      // Terminate the application when user presses ^Q anywhere inside the
      // main window.
      if (keyval == GDK_KEY_q)
      {
        std::exit(EXIT_SUCCESS);

        return true;
      }
      // Clear the line buffer when user presses ^L anywhere inside the main
      // window.
      else if (keyval == GDK_KEY_l)
      {
        m_line_display.clear();

        return true;
      }
    }
    else if (
      (state & Gdk::ModifierType::ALT_MASK) ==
      Gdk::ModifierType::ALT_MASK
    )
    {
      // Switch to stack view when Alt+1 is pressed.
      if (keyval == GDK_KEY_1)
      {
        m_notebook.set_current_page(0);

        return true;
      }
      // Switch to dictionary view when Alt+2 is pressed.
      else if (keyval == GDK_KEY_2)
      {
        m_notebook.set_current_page(1);

        return true;
      }
    }
    // Scroll line display up when Page Up is pressed.
    else if (keyval == GDK_KEY_Page_Up)
    {
      m_line_display.scroll_up();

      return true;
    }
    // Scroll line display down when Page Down is pressed.
    else if (keyval == GDK_KEY_Page_Down)
    {
      m_line_display.scroll_down();

      return true;
    }

    return false;
  }

  void
  Window::on_word_activated(
    const Glib::ustring& id,
    const Glib::ustring& value_source
  )
  {
    m_line_editor.set_text(value_source + " -> " + id);
    m_line_editor.grab_focus();
  }
}
