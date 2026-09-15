/*
 * Copyright (c) 2018-2026, Rauli Laine
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
#include <cstdio>
#include <cstring>
#include <stack>
#include <string>

#include <isocline.h>

#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/syntax.hpp"

#if !defined(BUFSIZ)
# define BUFSIZ 1024
#endif

namespace laskin::cli
{
  static int line_counter = 0;
  static std::stack<char32_t> open_braces;

  static const char* get_prompt(context&);
  static bool cursor_outside_string_or_comment(
    const char* line,
    std::size_t pos
  );
  static bool is_laskin_symbol_char(const char* s, long len);
  static void complete_dictionary_words(
    ic_completion_env_t* cenv,
    const char* prefix
  );
  static void complete_dictionary(
    ic_completion_env_t* cenv,
    const char* input
  );
  static void define_highlight_styles();
  static const char* highlight_style(laskin::syntax::highlight_kind kind);
  static void highlight_input(
    ic_highlight_env_t* henv,
    const char* input,
    void* arg
  );

  void
  run_repl(class context& context)
  {
    std::string source;

    ic_set_prompt_marker("", nullptr);
    ic_enable_multiline(false);
    ic_set_history(nullptr, -1);
    define_highlight_styles();
    ic_set_default_completer(complete_dictionary, &context);
    ic_set_default_highlighter(highlight_input, &context);

    for (;;)
    {
      char* input = ic_readline(get_prompt(context));

      if (!input)
      {
        break;
      }

      const std::string line(input);

      ic_free(input);
      source.append(line).append(1, '\n');
      syntax::count_open_braces(open_braces, line);

      if (!open_braces.empty())
      {
        continue;
      }

      try
      {
        context.run(source, &std::cout, "<repl>", line_counter);
      }
      catch (const error& e)
      {
        if (e.is(laskin::error::type::exit))
        {
          std::exit(EXIT_FAILURE);
        } else {
          std::cout << e << std::endl;
        }
      }

      source.clear();
    }
  }

  static const char*
  get_prompt(class context& context)
  {
    static char buffer[BUFSIZ];

    std::snprintf(
      buffer,
      BUFSIZ,
#if defined(_WIN32)
      "laskin:%03d:%llu%c ",
#else
      "laskin:%03d:%lu%c ",
#endif
      ++line_counter,
      context.data.size(),
      open_braces.empty() ? '>' : '*'
    );

    return buffer;
  }

  static bool
  cursor_outside_string_or_comment(const char* line, const std::size_t pos)
  {
    bool in_string = false;
    char quote = 0;

    for (std::size_t i = 0; i < pos; ++i)
    {
      if (!in_string && line[i] == '#')
      {
        return false;
      }

      if (!in_string && (line[i] == '"' || line[i] == '\''))
      {
        in_string = true;
        quote = line[i];
        continue;
      }

      if (in_string)
      {
        if (line[i] == '\\' && i + 1 < pos)
        {
          ++i;
          continue;
        }

        if (line[i] == quote)
        {
          in_string = false;
        }
      }
    }

    return !in_string;
  }

  static bool
  is_laskin_symbol_char(const char* s, const long len)
  {
    if (len <= 0)
    {
      return false;
    }

    char32_t c = 0;
    std::size_t pos = 0;

    if (
      !peelo::unicode::encoding::utf8::decode_advance(
        s,
        pos,
        static_cast<std::size_t>(len),
        c
      )
    )
    {
      return false;
    }

    return syntax::is_symbol(c);
  }

  static void
  complete_dictionary_words(
    ic_completion_env_t* cenv,
    const char* prefix
  )
  {
    using peelo::unicode::encoding::utf8::encode;

    const auto* context = static_cast<const laskin::context*>(
      ic_completion_arg(cenv)
    );
    const std::u32string prefix_u32 = peelo::unicode::encoding::utf8::decode(
      prefix
    );

    for (const auto& entry : context->dictionary)
    {
      if (entry.first.compare(0, prefix_u32.size(), prefix_u32) == 0)
      {
        if (!ic_add_completion(cenv, encode(entry.first).c_str()))
        {
          break;
        }
      }
    }
  }

  static void
  complete_dictionary(
    ic_completion_env_t* cenv,
    const char* input
  )
  {
    const auto length = std::strlen(input);

    if (!cursor_outside_string_or_comment(input, length))
    {
      return;
    }

    ic_complete_word(
      cenv,
      input,
      complete_dictionary_words,
      is_laskin_symbol_char
    );
  }

  static void
  define_highlight_styles()
  {
    ic_style_def("laskin-comment", "color=#6a9955");
    ic_style_def("laskin-string", "color=#ce9178");
    ic_style_def("laskin-number", "color=#b5cea8");
    ic_style_def("laskin-delimiter", "color=#ffd700");
    ic_style_def("laskin-symbol", "color=#c586c0");
  }

  static const char*
  highlight_style(const laskin::syntax::highlight_kind kind)
  {
    switch (kind)
    {
    case laskin::syntax::highlight_kind::comment:
      return "laskin-comment";

    case laskin::syntax::highlight_kind::string:
      return "laskin-string";

    case laskin::syntax::highlight_kind::number:
      return "laskin-number";

    case laskin::syntax::highlight_kind::delimiter:
      return "laskin-delimiter";

    case laskin::syntax::highlight_kind::symbol:
      return "laskin-symbol";
    }

    return nullptr;
  }

  static void
  highlight_input(
    ic_highlight_env_t* henv,
    const char* input,
    void* arg
  )
  {
    const auto* context = static_cast<const laskin::context*>(arg);
    const auto length = std::strlen(input);
    const std::u32string source = peelo::unicode::encoding::utf8::decode(
      input,
      length
    );

    laskin::syntax::highlight_source_line(
      source,
      [&](
        const std::size_t start,
        const std::size_t span_length,
        const laskin::syntax::highlight_kind kind
      )
      {
        const auto [byte_start, byte_length] =
          laskin::syntax::utf8_codepoint_range_to_bytes(
            input,
            length,
            start,
            span_length
          );

        ic_highlight(henv, static_cast<long>(byte_start), static_cast<long>(byte_length), highlight_style(kind));
      },
      [context](const std::u32string& word) {
        return context->dictionary.contains(word);
      }
    );
  }
}
