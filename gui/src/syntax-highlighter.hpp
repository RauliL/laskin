/*
 * Copyright (c) 2026, Rauli Laine
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

#include <gtkmm.h>

namespace laskin::gui
{
  /**
   * Applies Laskin syntax highlighting to a Gtk::TextBuffer using TextTags.
   * Token rules mirror editor/laskin.vim.
   */
  class SyntaxHighlighter
  {
  public:
    enum class Tag
    {
      COMMENT,
      STRING,
      BOOLEAN,
      CONSTANT,
      KEYWORD,
      NUMBER,
      OPERATOR,
      DELIMITER,
      DEFINITION,
      COUNT
    };

    explicit SyntaxHighlighter(const Glib::RefPtr<Gtk::TextBuffer>& buffer);

    void highlight_line(const Gtk::TextIter& line_start);
    void highlight_range(const Gtk::TextIter& start, const Gtk::TextIter& end);

  private:
    void create_tags();

    void apply_tag(
      Tag tag,
      Gtk::TextIter start,
      Gtk::TextIter end
    );

    void highlight_text(
      const Glib::ustring& line,
      const Gtk::TextIter& line_start
    );

    Glib::RefPtr<Gtk::TextBuffer> m_buffer;
    Glib::RefPtr<Gtk::TextTag> m_tags[static_cast<int>(Tag::COUNT)];
  };
}
