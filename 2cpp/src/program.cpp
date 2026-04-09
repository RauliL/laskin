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
#include <laskin/error.hpp>

#include "./program.hpp"

namespace laskin2cpp
{
  static inline bool
  is_valid(const std::shared_ptr<laskin::node>& node)
  {
    enum laskin::node::type type;

    if (!node)
    {
      return false;
    }
    type = node->type();

    return type == laskin::node::type::literal ||
      type == laskin::node::type::record_literal ||
      type == laskin::node::type::symbol ||
      type == laskin::node::type::vector_literal;
  }

  static void
  validate(const std::shared_ptr<laskin::node::record_literal>& record)
  {
    for (const auto& property : record->properties)
    {
      if (!is_valid(property.second))
      {
        throw laskin::error(
          laskin::error::type::syntax,
          U"Cannot transpile record literal.",
          record->position
        );
      }
    }
  }

  static void
  validate(const std::shared_ptr<laskin::node::vector_literal>& vector)
  {
    for (const auto& element : vector->elements)
    {
      if (!is_valid(element))
      {
        throw laskin::error(
          laskin::error::type::syntax,
          U"Cannot transpile vector literal.",
          vector->position
        );
      }
    }
  }

  void
  program::compile(const laskin::quote& quote)
  {
    for (const auto& node : quote.nodes())
    {
      if (!node)
      {
        continue;
      }
      switch (node->type())
      {
        case laskin::node::type::definition:
          m_instructions.push_back(std::make_shared<instruction::define>(
            node->position,
            std::static_pointer_cast<laskin::node::definition>(node)->id
          ));
          break;

        case laskin::node::type::literal:
          m_instructions.push_back(std::make_shared<instruction::push>(
            node->position,
            std::static_pointer_cast<laskin::node::literal>(node)->value
          ));
          break;

        case laskin::node::type::record_literal:
          {
            const auto record
              = std::static_pointer_cast<laskin::node::record_literal>(node);

            validate(record);
            m_instructions.push_back(std::make_shared<instruction::push>(
              record->position,
              record
            ));
          }
          break;

        case laskin::node::type::symbol:
          m_instructions.push_back(std::make_shared<instruction::lookup>(
            node->position,
            std::static_pointer_cast<laskin::node::symbol>(node)->id
          ));
          break;

        case laskin::node::type::vector_literal:
          {
            const auto vector
              = std::static_pointer_cast<laskin::node::vector_literal>(node);

            validate(vector);
            m_instructions.push_back(std::make_shared<instruction::push>(
              vector->position,
              vector
            ));
          }
          break;
      }
    }
  }

  void
  program::transpile(
    class writer& writer,
    const struct options& options
  ) const
  {
    for (const auto& instruction : m_instructions)
    {
      instruction->transpile(writer, options);
    }
    writer.flush();
  }
}
