/*
 * Copyright (c) 2018, Rauli Laine
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
#ifndef LASKIN_AST_HPP_GUARD
#define LASKIN_AST_HPP_GUARD

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
    class symbol;
    class vector_literal;

    explicit node(int line = 0, int column = 0);
    virtual ~node();

    node(const node&) = delete;
    node(node&&) = delete;
    void operator=(const node&) = delete;
    void operator=(node&&) = delete;

    inline int line() const
    {
      return m_line;
    }

    inline int column() const
    {
      return m_column;
    }

    /**
     * Executes the AST node as a statement.
     *
     * \param context
     * \param out
     */
    virtual void exec(class context& context, std::ostream& out) const = 0;

    /**
     * Evaluates the AST node as an expression.
     *
     * \param context
     * \param out
     */
    virtual value eval(class context& context, std::ostream& out) const = 0;

    /**
     * Constructs the most appropriate representation of what this AST node
     * would look like in source code.
     */
    virtual std::u32string to_source() const = 0;

  private:
    /** Line number where the AST node was encountered in. */
    const int m_line;
    /** Column number where the AST node was encountered in. */
    const int m_column;
  };

  class node::literal : public node
  {
  public:
    explicit literal(const class value& value, int line = 0, int column = 0);

    void exec(class context& context, std::ostream& out) const;
    value eval(class context& context, std::ostream& out) const;
    std::u32string to_source() const;

  private:
    const value m_value;
  };

  class node::vector_literal : public node
  {
  public:
    using container_type = std::vector<std::shared_ptr<node>>;

    explicit vector_literal(
      const container_type& elements,
      int line = 0,
      int column = 0
    );

    void exec(class context& context, std::ostream& out) const;
    value eval(class context& context, std::ostream& out) const;
    std::u32string to_source() const;

  private:
    const container_type m_elements;
  };

  class node::symbol : public node
  {
  public:
    explicit symbol(const std::u32string& id, int line = 0, int column = 0);

    inline const std::u32string& id() const
    {
      return m_id;
    }

    void exec(class context& context, std::ostream& out) const;
    value eval(class context& context, std::ostream& out) const;
    std::u32string to_source() const;

  private:
    const std::u32string m_id;
  };

  class node::definition : public node
  {
  public:
    explicit definition(
      const std::u32string& id,
      int line = 0,
      int column = 0
    );

    void exec(class context& context, std::ostream& out) const;
    value eval(class context& context, std::ostream& out) const;
    std::u32string to_source() const;

  private:
    const std::u32string m_id;
  };
}

#endif /* !LASKIN_AST_HPP_GUARD */
