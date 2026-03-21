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
#pragma once

#include <memory>

#include "laskin/value.hpp"

namespace laskin
{
  class context;
  class value;

  class node
  {
  public:
    class definition;
    class literal;
    class record_literal;
    class symbol;
    class vector_literal;

    /** Line number where the AST node was encountered in. */
    const int line;
    /** Column number where the AST node was encountered in. */
    const int column;

    explicit node(int line_ = 0, int column_ = 0)
      : line(line_)
      , column(column_) {}

    node(const node&) = delete;
    node(node&&) = delete;
    void operator=(const node&) = delete;
    void operator=(node&&) = delete;

    /**
     * Executes the AST node as a statement.
     *
     * \param context Program context to use for execution.
     * \param out Output stream for I/O.
     */
    virtual void exec(
      class context& context,
      std::ostream& out
    ) const = 0;

    /**
     * Evaluates the AST node as an expression.
     *
     * \param context Program context to use for evaluation.
     * \param out Output stream for I/O.
     */
    virtual value eval(
      class context& context,
      std::ostream& out
    ) const = 0;

    /**
     * Constructs the most appropriate representation of what this AST node
     * would look like in source code.
     */
    virtual std::u32string to_source() const = 0;
  };

  class node::literal final : public node
  {
  public:
    const class value value;

    explicit literal(
      const class value& value_,
      int line = 0,
      int column = 0
    )
      : node(line, column)
      , value(value_) {}

    void exec(
      class context& context,
      std::ostream& out
    ) const override;

    inline class value eval(
      class context& context,
      std::ostream& out
    ) const override
    {
      return value;
    }

    inline std::u32string to_source() const override
    {
      return value.to_source();
    }
  };

  class node::vector_literal final : public node
  {
  public:
    using container_type = std::vector<std::shared_ptr<node>>;

    const container_type elements;

    explicit vector_literal(
      const container_type& elements_,
      int line = 0,
      int column = 0
    )
      : node(line, column)
      , elements(elements_) {}

    void exec(
      class context& context,
      std::ostream& out
    ) const override;

    value eval(
      class context& context,
      std::ostream& out
    ) const override;

    std::u32string to_source() const override;
  };

  class node::record_literal final : public node
  {
  public:
    using container_type = tsl::ordered_map<
      std::u32string,
      std::shared_ptr<node>
    >;

    const container_type properties;

    explicit record_literal(
      const container_type& properties_,
      int line = 0,
      int column = 0
    )
      : node(line, column)
      , properties(properties_) {}

    void exec(
      class context& context,
      std::ostream& out
    ) const override;

    value eval(
      class context& context,
      std::ostream& out
    ) const override;

    std::u32string to_source() const override;
  };

  class node::symbol final : public node
  {
  public:
    const std::u32string id;

    explicit symbol(const std::u32string& id_, int line = 0, int column = 0)
      : node(line, column)
      , id(id_) {}

    void exec(
      class context& context,
      std::ostream& out
    ) const override;

    value eval(
      class context& context,
      std::ostream& out
    ) const override;

    inline std::u32string to_source() const override
    {
      return id;
    }
  };

  class node::definition final : public node
  {
  public:
    const std::u32string id;

    explicit definition(
      const std::u32string& id_,
      int line = 0,
      int column = 0
    )
      : node(line, column)
      , id(id_) {}

    void exec(
      class context& context,
      std::ostream& out
    ) const override;

    value eval(
      class context& context,
      std::ostream& out
    ) const override;

    inline std::u32string to_source() const override
    {
      return U"-> " + id;
    }
  };
}
