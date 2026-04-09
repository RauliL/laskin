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

#include <variant>

#include <laskin/ast.hpp>
#include <laskin/value.hpp>

#include "./options.hpp"
#include "./writer.hpp"

namespace laskin2cpp
{
  class instruction
  {
  public:
    class define;
    class lookup;
    class push;

    using push_value = std::variant<
      laskin::value,
      std::shared_ptr<laskin::node>
    >;

    enum class type
    {
      define,
      lookup,
      push,
    };

    const std::optional<laskin::position> position;

    explicit instruction(const std::optional<laskin::position>& position_)
      : position(position_) {}

    LASKIN_DISALLOW_COPY_AND_ASSIGN(instruction);

    virtual enum type type() const = 0;

    virtual void transpile(
      class writer& writer,
      const struct options& options
    ) const = 0;
  };

  class instruction::define final : public instruction
  {
  public:
    const std::u32string id;
    const std::optional<push_value> value;

    explicit define(
      const std::optional<laskin::position>& position_,
      const std::u32string& id_,
      const std::optional<push_value>& value_ = std::nullopt
    )
      : instruction(position_)
      , id(id_)
      , value(value_) {}

    inline enum type type() const override
    {
      return type::define;
    }

    void transpile(
      class writer& writer,
      const struct options& options
    ) const override;
  };

  class instruction::lookup final : public instruction
  {
  public:
    const std::u32string id;

    explicit lookup(
      const std::optional<laskin::position>& position_,
      const std::u32string& id_
    )
      : instruction(position_)
      , id(id_) {}

    inline enum type type() const override
    {
      return type::lookup;
    }

    void transpile(
      class writer& writer,
      const struct options& options
    ) const override;
  };

  class instruction::push final : public instruction
  {
  public:
    const push_value value;

    explicit push(
      const std::optional<laskin::position>& position_,
      const push_value& value_
    )
      : instruction(position_)
      , value(value_) {}

    inline enum type type() const override
    {
      return type::push;
    }

    void transpile(
      class writer& writer,
      const struct options& options
    ) const override;
  };
}
