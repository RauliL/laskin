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

#include <laskin/gui/utils.hpp>
#include <laskin/gui/window.hpp>

namespace laskin::gui
{
  const int Window::DEFAULT_WIDTH = 500;
  const int Window::DEFAULT_HEIGHT = 300;

  Window::Window(const Glib::RefPtr<Context>& context)
    : m_context(context)
    , m_box(Gtk::ORIENTATION_VERTICAL)
  {
    set_title("Laskin");
    set_border_width(5);
    set_default_size(DEFAULT_WIDTH, DEFAULT_HEIGHT);

    m_paned.pack1(m_line_display, true, false);
    m_paned.pack2(m_notebook, false, false);

    m_notebook.append_page(m_stack_display, "Stack");
    m_notebook.append_page(m_dictionary_display, "Dictionary");

    m_box.pack_start(m_paned);
    m_box.pack_start(m_line_editor, Gtk::PACK_SHRINK);

    add(m_box);
    show_all_children();

    m_dictionary_display.update(m_context->dictionary());

    m_line_editor.signal_line_received().connect(sigc::mem_fun(
      this,
      &Window::on_line_received
    ));
    m_context->signal_error_thrown().connect(sigc::mem_fun(
      this,
      &Window::on_error_thrown
    ));
    m_context->signal_text_written().connect(sigc::mem_fun(
      this,
      &Window::on_text_written
    ));
    m_dictionary_display.signal_word_activated().connect(sigc::mem_fun(
      this,
      &Window::on_word_activated
    ));
  }

  void Window::on_show()
  {
    Gtk::Window::on_show();
    m_line_editor.grab_focus();
  }

  void Window::on_line_received(const Glib::ustring& line)
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

  void Window::on_error_thrown(const laskin::error& error)
  {
    std::stringstream buffer;

    buffer << error;
    m_line_display.add_line(buffer.str() + '\n', LineDisplay::LINE_TYPE_ERROR);
  }

  void Window::on_text_written(const Glib::ustring& text)
  {
    m_line_display.add_line(text, LineDisplay::LINE_TYPE_OUTPUT);
  }

  bool Window::on_key_press_event(GdkEventKey* event)
  {
    if ((event->state & GDK_CONTROL_MASK) != 0)
    {
      // Terminate the application when user presses ^Q anywhere inside the
      // main window.
      if (event->keyval == GDK_KEY_q)
      {
        std::exit(EXIT_SUCCESS);

        return true;
      }
      // Clear the line buffer when user presses ^L anywhere inside the main
      // window.
      else if (event->keyval == GDK_KEY_l)
      {
        m_line_display.clear();

        return true;
      }
    }
    else if ((event->state & GDK_MOD1_MASK) != 0)
    {
      // Switch to stack view when Mod1+1 is pressed.
      if (event->keyval == GDK_KEY_1)
      {
        m_notebook.set_current_page(0);

        return true;
      }
      // Switch to dictionary view when Mod1+2 is pressed.
      else if (event->keyval == GDK_KEY_2)
      {
        m_notebook.set_current_page(1);

        return true;
      }
    }

    return Gtk::Window::on_key_press_event(event);
  }

  void Window::on_word_activated(
    const Glib::ustring& id,
    const Glib::ustring& value_source
  )
  {
    m_line_editor.set_text(value_source + " -> " + id);
    m_line_editor.grab_focus();
  }
}
