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
#include "./instruction.hpp"
#include "./transpile.hpp"

namespace laskin2cpp
{
  static void
  transpile(
    const instruction::push_value& value,
    class writer& writer,
    const struct options& options
  )
  {
    if (std::holds_alternative<laskin::value>(value))
    {
      laskin2cpp::transpile(
        std::get<laskin::value>(value),
        writer,
        options
      );
    }
    else if (std::holds_alternative<std::shared_ptr<laskin::node>>(value))
    {
      laskin2cpp::transpile(
        std::get<std::shared_ptr<laskin::node>>(value),
        writer,
        options
      );
    }
  }

  void
  instruction::define::transpile(
    class writer& writer,
    const struct options& options
  ) const
  {
    writer.print("c.dictionary[" + writer::escape(id) + "] = ");
    if (value)
    {
      laskin2cpp::transpile(*value, writer, options);
    } else {
      writer.print("c.pop()");
    }
    writer.println(";");
  }

  void
  instruction::lookup::transpile(
    class writer& writer,
    const struct options& options
  ) const
  {
    if (options.number_optimization && laskin::number::is_valid(id))
    {
      writer.print("c.push(");
      transpile_number(laskin::number::parse(id), writer);
      writer.println(");");
    } else {
      writer.println("c.lookup(" + writer::escape(id) + ", &std::cout);");
    }
  }

  void
  instruction::push::transpile(
    class writer& writer,
    const struct options& options
  ) const
  {
    writer.print("c.push(");
    laskin2cpp::transpile(value, writer, options);
    writer.println(");");
  }
}
