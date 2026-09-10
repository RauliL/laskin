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

#include <peelo/number.hpp>

#include <algorithm>
#include <string>
#include <vector>

namespace laskin::gui
{
  namespace
  {
    inline bool
    is_alpha(gunichar c)
    {
      return (c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z');
    }

    inline bool
    is_alnum(gunichar c)
    {
      return is_alpha(c) || (c >= U'0' && c <= U'9');
    }

    inline bool
    is_identifier_char(gunichar c)
    {
      return is_alnum(c) || c == U'-';
    }

    inline bool
    is_word_char(gunichar c)
    {
      return is_alnum(c) || c == U'_' || c == U'.';
    }

    inline bool
    is_digit(gunichar c)
    {
      return c >= U'0' && c <= U'9';
    }

    inline bool
    starts_with(
      const Glib::ustring& line,
      Glib::ustring::size_type pos,
      const char* literal
    )
    {
      const Glib::ustring prefix(literal);

      if (pos + prefix.length() > line.length())
      {
        return false;
      }

      return line.compare(pos, prefix.length(), prefix) == 0;
    }

    inline bool
    word_boundary_after(
      const Glib::ustring& line,
      Glib::ustring::size_type pos
    )
    {
      if (pos >= line.length())
      {
        return true;
      }

      const auto c = line[pos];

      return !is_identifier_char(c);
    }

    inline bool
    word_boundary_before(
      const Glib::ustring& line,
      Glib::ustring::size_type pos
    )
    {
      if (pos == 0)
      {
        return true;
      }

      const auto c = line[pos - 1];

      return !is_identifier_char(c);
    }

    bool
    matches_any(const Glib::ustring& word, const char* const* keywords)
    {
      for (auto keyword = keywords; *keyword; ++keyword)
      {
        if (word == *keyword)
        {
          return true;
        }
      }

      return false;
    }

    bool
    match_keyword(const Glib::ustring& word, SyntaxHighlighter::Tag& tag)
    {
      static const char* booleans[] = { "true", "false", nullptr };
      static const char* constants[] = {
        "e",
        "pi",
        "inf",
        "nan",
        "january",
        "february",
        "march",
        "april",
        "may",
        "june",
        "july",
        "august",
        "september",
        "october",
        "november",
        "december",
        "sunday",
        "monday",
        "tuesday",
        "wednesday",
        "thursday",
        "friday",
        "saturday",
        nullptr
      };
      static const char* conditionals[] = {
        "if",
        "try",
        "if-else",
        "try-else",
        nullptr
      };
      static const char* repeats[] = { "while", nullptr };
      static const char* statements[] = {
        "quit",
        "lookup",
        "define",
        "delete",
        "symbols",
        "include",
        nullptr
      };
      static const char* stack_words[] = {
        "clear",
        "dup",
        "drop",
        "nip",
        "over",
        "rot",
        "swap",
        "tuck",
        "depth",
        nullptr
      };
      static const char* type_tests[] = {
        "boolean?",
        "date?",
        "month?",
        "number?",
        "vector?",
        "record?",
        "string?",
        "time?",
        "quote?",
        "weekday?",
        nullptr
      };

      if (matches_any(word, booleans))
      {
        tag = SyntaxHighlighter::Tag::BOOLEAN;
        return true;
      }

      if (matches_any(word, constants))
      {
        tag = SyntaxHighlighter::Tag::CONSTANT;
        return true;
      }

      if (
        matches_any(word, conditionals)
        || matches_any(word, repeats)
        || matches_any(word, statements)
      )
      {
        tag = SyntaxHighlighter::Tag::KEYWORD;
        return true;
      }

      if (matches_any(word, stack_words) || matches_any(word, type_tests))
      {
        tag = SyntaxHighlighter::Tag::OPERATOR;
        return true;
      }

      return false;
    }

    const std::vector<std::string>&
    unit_symbols()
    {
      static const std::vector<std::string> symbols = []() {
        std::vector<std::string> result;

        for (const auto type : {
          peelo::number::unit::type::length,
          peelo::number::unit::type::mass,
          peelo::number::unit::type::time
        })
        {
          for (const auto& u : peelo::number::unit::all_units_of(type))
          {
            result.push_back(u.symbol);
          }
        }

        std::sort(
          result.begin(),
          result.end(),
          [](const std::string& a, const std::string& b) {
            return a.size() > b.size();
          }
        );

        return result;
      }();

      return symbols;
    }

    bool
    match_unit(
      const Glib::ustring& line,
      Glib::ustring::size_type pos,
      Glib::ustring::size_type& end
    )
    {
      for (const auto& unit_str : unit_symbols())
      {
        if (
          starts_with(line, pos, unit_str.c_str())
          && word_boundary_after(line, pos + unit_str.length())
        )
        {
          end = pos + unit_str.length();

          return true;
        }
      }

      return false;
    }

    bool
    parse_number_body(
      const Glib::ustring& line,
      Glib::ustring::size_type pos,
      Glib::ustring::size_type& end
    )
    {
      if (pos >= line.length() || !is_digit(line[pos]))
      {
        return false;
      }

      end = pos;

      while (end < line.length() && is_digit(line[end]))
      {
        ++end;
      }

      while (
        end < line.length()
        && line[end] == U'_'
        && end + 1 < line.length()
        && is_digit(line[end + 1])
      )
      {
        ++end;

        while (end < line.length() && is_digit(line[end]))
        {
          ++end;
        }
      }

      if (
        end < line.length()
        && line[end] == U'.'
        && end + 1 < line.length()
        && is_digit(line[end + 1])
      )
      {
        ++end;

        while (end < line.length() && is_digit(line[end]))
        {
          ++end;
        }

        while (
          end < line.length()
          && line[end] == U'_'
          && end + 1 < line.length()
          && is_digit(line[end + 1])
        )
        {
          ++end;

          while (end < line.length() && is_digit(line[end]))
          {
            ++end;
          }
        }
      }

      Glib::ustring::size_type unit_end = end;

      if (match_unit(line, end, unit_end))
      {
        end = unit_end;
      }

      return end > pos;
    }

    bool
    parse_number(
      const Glib::ustring& line,
      Glib::ustring::size_type pos,
      Glib::ustring::size_type& end
    )
    {
      if (pos >= line.length())
      {
        return false;
      }

      const auto prev = pos > 0 ? line[pos - 1] : 0;
      const bool after_dot = prev == U'.';

      if (after_dot)
      {
        return false;
      }

      Glib::ustring::size_type body_start = pos;
      Glib::ustring::size_type body_end = pos;

      if (line[pos] == U'+' || line[pos] == U'-')
      {
        const auto sign = line[pos];

        if (sign == U'-' && starts_with(line, pos, "-inf"))
        {
          end = pos + Glib::ustring("-inf").length();
          return word_boundary_after(line, end);
        }

        if (sign == U'+' && !parse_number_body(line, pos + 1, body_end))
        {
          return false;
        }

        if (sign == U'-')
        {
          if (!parse_number_body(line, pos + 1, body_end))
          {
            return false;
          }
        }

        if (sign == U'+')
        {
          body_start = pos + 1;
        } else {
          body_start = pos + 1;
        }

        if (body_end > body_start)
        {
          end = body_end;

          return true;
        }

        return false;
      }

      if (parse_number_body(line, pos, body_end))
      {
        end = body_end;

        return true;
      }

      return false;
    }

    Glib::ustring::size_type
    read_word(
      const Glib::ustring& line,
      Glib::ustring::size_type pos
    )
    {
      const auto start = pos;

      while (pos < line.length() && is_identifier_char(line[pos]))
      {
        ++pos;
      }

      return pos - start;
    }
  }

  SyntaxHighlighter::SyntaxHighlighter(
    const Glib::RefPtr<Gtk::TextBuffer>& buffer
  )
    : m_buffer(buffer)
  {
    create_tags();
  }

  void
  SyntaxHighlighter::create_tags()
  {
    m_tags[static_cast<int>(Tag::COMMENT)] =
      m_buffer->create_tag("laskin-comment");
    m_tags[static_cast<int>(Tag::STRING)] =
      m_buffer->create_tag("laskin-string");
    m_tags[static_cast<int>(Tag::BOOLEAN)] =
      m_buffer->create_tag("laskin-boolean");
    m_tags[static_cast<int>(Tag::CONSTANT)] =
      m_buffer->create_tag("laskin-constant");
    m_tags[static_cast<int>(Tag::KEYWORD)] =
      m_buffer->create_tag("laskin-keyword");
    m_tags[static_cast<int>(Tag::NUMBER)] =
      m_buffer->create_tag("laskin-number");
    m_tags[static_cast<int>(Tag::OPERATOR)] =
      m_buffer->create_tag("laskin-operator");
    m_tags[static_cast<int>(Tag::DELIMITER)] =
      m_buffer->create_tag("laskin-delimiter");
    m_tags[static_cast<int>(Tag::DEFINITION)] =
      m_buffer->create_tag("laskin-definition");

    m_tags[static_cast<int>(Tag::COMMENT)]->property_foreground().set_value("#6a9955");
    m_tags[static_cast<int>(Tag::STRING)]->property_foreground().set_value("#ce9178");
    m_tags[static_cast<int>(Tag::BOOLEAN)]->property_foreground().set_value("#569cd6");
    m_tags[static_cast<int>(Tag::CONSTANT)]->property_foreground().set_value("#4fc1ff");
    m_tags[static_cast<int>(Tag::KEYWORD)]->property_foreground().set_value("#c586c0");
    m_tags[static_cast<int>(Tag::NUMBER)]->property_foreground().set_value("#b5cea8");
    m_tags[static_cast<int>(Tag::OPERATOR)]->property_foreground().set_value("#d4d4d4");
    m_tags[static_cast<int>(Tag::DELIMITER)]->property_foreground().set_value("#ffd700");
    m_tags[static_cast<int>(Tag::DEFINITION)]->property_foreground().set_value("#dcdcaa");
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
    Glib::ustring::size_type pos = 0;
    const auto length = line.length();

    while (pos < length)
    {
      const auto c = line[pos];

      if (c == U'#')
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = line_start;

        tag_end.forward_chars(static_cast<int>(length));
        apply_tag(Tag::COMMENT, tag_start, tag_end);
        break;
      }

      if (c == U'"' || c == U'\'')
      {
        const auto quote = c;
        const auto string_start = pos;

        ++pos;

        while (pos < length)
        {
          if (line[pos] == quote)
          {
            ++pos;
            break;
          }

          if (line[pos] == U'\\' && pos + 1 < length)
          {
            pos += 2;
          } else {
            ++pos;
          }
        }

        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(string_start));
        auto tag_end = line_start;

        tag_end.forward_chars(static_cast<int>(pos));
        apply_tag(Tag::STRING, tag_start, tag_end);
        continue;
      }

      if (c == U'-' && starts_with(line, pos, "->"))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(2);
        apply_tag(Tag::DEFINITION, tag_start, tag_end);
        pos += 2;
        continue;
      }

      if (c == U'(' || c == U')' || c == U'[' || c == U']' || c == U'{' || c == U'}')
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(1);
        apply_tag(Tag::DELIMITER, tag_start, tag_end);
        ++pos;
        continue;
      }

      if (starts_with(line, pos, ".."))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(2);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        pos += 2;
        continue;
      }

      if (starts_with(line, pos, ">string"))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(7);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        pos += 7;
        continue;
      }

      if (starts_with(line, pos, ">source"))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(7);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        pos += 7;
        continue;
      }

      if (starts_with(line, pos, "<>"))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(2);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        pos += 2;
        continue;
      }

      if (starts_with(line, pos, ">="))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(2);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        pos += 2;
        continue;
      }

      if (starts_with(line, pos, "<="))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(2);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        pos += 2;
        continue;
      }

      if (c == U'=' && (pos + 1 >= length || line[pos + 1] != U'='))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(1);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        ++pos;
        continue;
      }

      if (
        c == U'>'
        && (pos + 1 >= length || (line[pos + 1] != U'=' && !is_alpha(line[pos + 1])))
      )
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(1);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        ++pos;
        continue;
      }

      if (
        c == U'<'
        && (pos + 1 >= length || (line[pos + 1] != U'=' && line[pos + 1] != U'>'))
      )
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(1);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        ++pos;
        continue;
      }

      if (c == U'*' || c == U'/' || c == U'%')
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(1);
        apply_tag(Tag::OPERATOR, tag_start, tag_end);
        ++pos;
        continue;
      }

      if (c == U'-')
      {
        const bool after_word = pos > 0 && is_word_char(line[pos - 1]);
        const bool before_number = (
          is_digit(line[pos + 1])
          || starts_with(line, pos + 1, "inf")
        );
        const bool before_gt = pos + 1 < length && line[pos + 1] == U'>';

        if (after_word && !before_number && !before_gt)
        {
          auto tag_start = line_start;

          tag_start.forward_chars(static_cast<int>(pos));
          auto tag_end = tag_start;

          tag_end.forward_chars(1);
          apply_tag(Tag::OPERATOR, tag_start, tag_end);
          ++pos;
          continue;
        }
      }

      if (c == U'+')
      {
        const bool after_word = pos > 0 && is_word_char(line[pos - 1]);
        const bool before_number = is_digit(line[pos + 1]);

        if (after_word && !before_number)
        {
          auto tag_start = line_start;

          tag_start.forward_chars(static_cast<int>(pos));
          auto tag_end = tag_start;

          tag_end.forward_chars(1);
          apply_tag(Tag::OPERATOR, tag_start, tag_end);
          ++pos;
          continue;
        }
      }

      if (c == U'.')
      {
        if (starts_with(line, pos, ".s") && word_boundary_after(line, pos + 2))
        {
          auto tag_start = line_start;

          tag_start.forward_chars(static_cast<int>(pos));
          auto tag_end = tag_start;

          tag_end.forward_chars(2);
          apply_tag(Tag::OPERATOR, tag_start, tag_end);
          pos += 2;
          continue;
        }

        const bool after_digit = pos > 0 && is_digit(line[pos - 1]);

        if (!after_digit)
        {
          auto tag_start = line_start;

          tag_start.forward_chars(static_cast<int>(pos));
          auto tag_end = tag_start;

          tag_end.forward_chars(1);
          apply_tag(Tag::OPERATOR, tag_start, tag_end);
          ++pos;
          continue;
        }
      }

      if (c == U'-' && starts_with(line, pos, "-inf"))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = tag_start;

        tag_end.forward_chars(4);
        apply_tag(Tag::CONSTANT, tag_start, tag_end);
        pos += 4;
        continue;
      }

      Glib::ustring::size_type number_end = pos;

      if (parse_number(line, pos, number_end))
      {
        auto tag_start = line_start;

        tag_start.forward_chars(static_cast<int>(pos));
        auto tag_end = line_start;

        tag_end.forward_chars(static_cast<int>(number_end));
        apply_tag(Tag::NUMBER, tag_start, tag_end);
        pos = number_end;
        continue;
      }

      if (is_identifier_char(c) && word_boundary_before(line, pos))
      {
        const auto word_len = read_word(line, pos);

        if (word_len > 0)
        {
          const Glib::ustring word = line.substr(pos, word_len);
          Tag tag;

          if (match_keyword(word, tag))
          {
            auto tag_start = line_start;

            tag_start.forward_chars(static_cast<int>(pos));
            auto tag_end = line_start;

            tag_end.forward_chars(static_cast<int>(pos + word_len));
            apply_tag(tag, tag_start, tag_end);
          }

          pos += word_len;
          continue;
        }
      }

      ++pos;
    }
  }
}
