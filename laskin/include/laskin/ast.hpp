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

#include "laskin/position.hpp"
#include "laskin/value.hpp"

namespace laskin
{
  class context;
  class value;

  /**
   * Representation of an AST node.
   */
  class node
  {
  public:
    enum class type
    {
      definition,
      literal,
      record_literal,
      symbol,
      vector_literal,
    };

    class definition;
    class literal;
    class record_literal;
    class symbol;
    class vector_literal;

    /** Position of the AST node in the source code. */
    std::optional<struct position> position;

    explicit node(const std::optional<struct position>& position_)
      : position(position_) {}

    node(const node&) = delete;
    node(node&&) = delete;
    void operator=(const node&) = delete;
    void operator=(node&&) = delete;

    /**
     * Returns type of the AST node.
     */
    virtual enum type type() const = 0;

    /**
     * Executes the AST node as a statement.
     *
     * \param context Program context to use for execution.
     * \param out Output stream for I/O.
     */
    virtual void exec(
      class context& context,
      std::ostream* out
    ) const = 0;

    /**
     * Evaluates the AST node as an expression.
     *
     * \param context Program context to use for evaluation.
     * \param out Output stream for I/O.
     */
    virtual value eval(
      class context& context,
      std::ostream* out
    ) const = 0;

    /**
     * Tests whether two AST nodes are equal.
     */
    static inline bool equals(
      const std::shared_ptr<node>& a,
      const std::shared_ptr<node>& b
    )
    {
      return a ? a->equals(b) : !b;
    }

    /**
     * Tests whether node is equal with another node.
     */
    virtual bool equals(const std::shared_ptr<node>& that) const = 0;

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
      const std::optional<struct position>& position_ = std::nullopt
    )
      : node(position_)
      , value(value_) {}

    inline enum type type() const override
    {
      return type::literal;
    }

    void exec(
      class context& context,
      std::ostream* out
    ) const override;

    inline class value eval(
      class context& context,
      std::ostream* out
    ) const override
    {
      return value;
    }

    bool equals(const std::shared_ptr<node>& that) const override;

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
      const std::optional<struct position>& position_ = std::nullopt
    )
      : node(position_)
      , elements(elements_) {}

    inline enum type type() const override
    {
      return type::vector_literal;
    }

    void exec(
      class context& context,
      std::ostream* out
    ) const override;

    value eval(
      class context& context,
      std::ostream* out
    ) const override;

    bool equals(const std::shared_ptr<node>& that) const override;

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
      const std::optional<struct position>& position_ = std::nullopt
    )
      : node(position_)
      , properties(properties_) {}

    inline enum type type() const override
    {
      return type::record_literal;
    }

    void exec(
      class context& context,
      std::ostream* out
    ) const override;

    value eval(
      class context& context,
      std::ostream* out
    ) const override;

    bool equals(const std::shared_ptr<node>& that) const override;

    std::u32string to_source() const override;
  };

  class node::symbol final : public node
  {
  public:
    const std::u32string id;

    explicit symbol(
      const std::u32string& id_,
      const std::optional<struct position>& position_ = std::nullopt
    )
      : node(position_)
      , id(id_) {}

    inline enum type type() const override
    {
      return type::symbol;
    }

    void exec(
      class context& context,
      std::ostream* out
    ) const override;

    value eval(
      class context& context,
      std::ostream* out
    ) const override;

    bool equals(const std::shared_ptr<node>& that) const override;

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
      const std::optional<struct position>& position_ = std::nullopt
    )
      : node(position_)
      , id(id_) {}

    inline enum type type() const override
    {
      return type::definition;
    }

    void exec(
      class context& context,
      std::ostream* out
    ) const override;

    value eval(
      class context& context,
      std::ostream* out
    ) const override;

    bool equals(const std::shared_ptr<node>& that) const override;

    inline std::u32string to_source() const override
    {
      return U"-> " + id;
    }
  };
}
