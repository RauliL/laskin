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
#include "./syntax-highlighter.hpp"

#include "./utils.hpp"

#include "laskin/syntax.hpp"

namespace laskin::gui
{
  namespace
  {
    SyntaxHighlighter::Tag
    tag_for_kind(laskin::syntax::highlight_kind kind)
    {
      switch (kind)
      {
      case laskin::syntax::highlight_kind::comment:
        return SyntaxHighlighter::Tag::COMMENT;

      case laskin::syntax::highlight_kind::string:
        return SyntaxHighlighter::Tag::STRING;

      case laskin::syntax::highlight_kind::number:
        return SyntaxHighlighter::Tag::NUMBER;

      case laskin::syntax::highlight_kind::delimiter:
        return SyntaxHighlighter::Tag::DELIMITER;

      case laskin::syntax::highlight_kind::symbol:
        return SyntaxHighlighter::Tag::SYMBOL;
      }

      return SyntaxHighlighter::Tag::COMMENT;
    }
  }

  SyntaxHighlighter::SyntaxHighlighter(
    const Glib::RefPtr<Gtk::TextBuffer>& buffer
  )
    : m_buffer(buffer)
    , m_dictionary(nullptr)
  {
    create_tags();
  }

  void
  SyntaxHighlighter::set_dictionary(
    const laskin::context::dictionary_type& dictionary
  )
  {
    m_dictionary = &dictionary;
  }

  void
  SyntaxHighlighter::create_tags()
  {
    m_tags[static_cast<int>(Tag::COMMENT)] =
      m_buffer->create_tag("laskin-comment");
    m_tags[static_cast<int>(Tag::STRING)] =
      m_buffer->create_tag("laskin-string");
    m_tags[static_cast<int>(Tag::NUMBER)] =
      m_buffer->create_tag("laskin-number");
    m_tags[static_cast<int>(Tag::DELIMITER)] =
      m_buffer->create_tag("laskin-delimiter");
    m_tags[static_cast<int>(Tag::SYMBOL)] =
      m_buffer->create_tag("laskin-symbol");

    m_tags[static_cast<int>(Tag::COMMENT)]->property_foreground().set_value("#6a9955");
    m_tags[static_cast<int>(Tag::STRING)]->property_foreground().set_value("#ce9178");
    m_tags[static_cast<int>(Tag::NUMBER)]->property_foreground().set_value("#b5cea8");
    m_tags[static_cast<int>(Tag::DELIMITER)]->property_foreground().set_value("#ffd700");
    m_tags[static_cast<int>(Tag::SYMBOL)]->property_foreground().set_value("#c586c0");
  }

  void
  SyntaxHighlighter::apply_tag(
    Tag tag,
    Gtk::TextIter start,
    Gtk::TextIter end
  )
  {
    if (start.get_offset() < end.get_offset())
    {
      m_buffer->apply_tag(m_tags[static_cast<int>(tag)], start, end);
    }
  }

  void
  SyntaxHighlighter::highlight_line(const Gtk::TextIter& line_start)
  {
    if (!line_start.ends_line())
    {
      auto line_end = line_start;

      line_end.forward_to_line_end();
      highlight_range(line_start, line_end);
    } else {
      highlight_range(line_start, line_start);
    }
  }

  void
  SyntaxHighlighter::highlight_range(
    const Gtk::TextIter& start,
    const Gtk::TextIter& end
  )
  {
    const auto line = m_buffer->get_text(start, end, false);

    m_buffer->remove_all_tags(start, end);
    highlight_text(line, start);
  }

  void
  SyntaxHighlighter::highlight_text(
    const Glib::ustring& line,
    const Gtk::TextIter& line_start
  )
  {
    const auto source = utils::string_convert<std::u32string>(line);
    const auto* dictionary = m_dictionary;

    laskin::syntax::highlight_source_line(
      source,
      [&](
        const std::size_t start,
        const std::size_t length,
        const laskin::syntax::highlight_kind kind
      )
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(start));
        auto tag_end = line_start;

        tag_end.forward_chars(static_cast<int>(start + length));
        apply_tag(tag_for_kind(kind), tag_start, tag_end);
      },
      dictionary
        ? laskin::syntax::dictionary_predicate(
          [dictionary](const std::u32string& word) {
            return dictionary->contains(word);
          }
        )
        : laskin::syntax::dictionary_predicate{}
    );
  }
}
