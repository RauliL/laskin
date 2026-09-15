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

#include <fstream>
#include <istream>
#include <memory>

#include <nlohmann/json.hpp>

#include "laskin/ast.hpp"
#include "laskin/context.hpp"
#include "laskin/value.hpp"

namespace laskin
{
  /**
   * Serializes a Laskin value into JSON matching the WebAssembly value schema.
   */
  void to_json(nlohmann::json& json, const value& value);

  /**
   * Deserializes a Laskin value from JSON matching the WebAssembly value schema.
   */
  void from_json(const nlohmann::json& json, value& value);

  /**
   * Serializes an AST node into JSON matching the WebAssembly AST schema.
   */
  void to_json(nlohmann::json& json, const std::shared_ptr<node>& node);

  /**
   * Deserializes an AST node from JSON matching the WebAssembly AST schema.
   */
  void from_json(const nlohmann::json& json, std::shared_ptr<node>& node);

  /**
   * Serializes interpreter context into JSON. The data stack is ordered with
   * the top of the stack at index 0. Dictionary entries whose value is a
   * native quote are omitted.
   */
  void to_json(nlohmann::json& json, const context& context);

  /**
   * Reloads interpreter context from JSON produced by `to_json`. Replaces the
   * data stack and all non-native dictionary entries; built-in native words
   * are preserved.
   */
  void from_json(const nlohmann::json& json, context& context);

  /**
   * Reads a JSON context snapshot from the given file and applies it with
   * `from_json`.
   */
  void load_snapshot(context& context, const std::filesystem::path& path);

  /**
   * Reads a JSON context snapshot from the given input stream and applies it
   * with `from_json`.
   */
  void load_snapshot(context& context, std::istream& input);
}
