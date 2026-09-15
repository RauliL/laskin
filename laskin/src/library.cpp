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
#include <unordered_set>

#include <dylib.hpp>
#include <peelo/unicode/encoding/utf8.hpp>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/plugin.hpp"

namespace laskin
{
  struct context::library_storage
  {
    std::vector<std::unique_ptr<dylib::library>> libraries;
    std::unordered_set<std::filesystem::path> loaded;
  };

  static bool
  is_dynamic_library_extension(const std::string& extension)
  {
    return extension == ".so"
      || extension == ".dll"
      || extension == ".dylib";
  }

  static std::string
  platform_library_extension()
  {
    return DYLIB_WIN_MAC_OTHER(".dll", ".dylib", ".so");
  }

  static std::filesystem::path
  strip_dynamic_library_extension(const std::filesystem::path& path)
  {
    if (is_dynamic_library_extension(path.extension().string()))
    {
      auto result = path;

      result.replace_extension();

      return result;
    }

    return path;
  }

  static std::filesystem::path
  normalize_library_lookup_path(std::filesystem::path path)
  {
    path = strip_dynamic_library_extension(path);

    if (!path.has_parent_path() || path.parent_path().empty())
    {
      path = std::filesystem::path(".") / path.filename();
    }

    return path;
  }

  static std::vector<std::filesystem::path>
  library_search_paths(const std::filesystem::path& path)
  {
    namespace fs = std::filesystem;

    const auto logical = normalize_library_lookup_path(path);
    const auto directory = logical.parent_path();
    const auto name = logical.filename().string();
    const auto suffix = platform_library_extension();
    std::vector<fs::path> candidates =
    {
      directory / (name + suffix),
      directory / ("lib" + name + suffix),
    };

    if (
      is_dynamic_library_extension(path.extension().string())
      && fs::exists(path)
    )
    {
      candidates.insert(std::begin(candidates), path);
    }

    return candidates;
  }

  static std::filesystem::path
  library_lookup_key(const std::filesystem::path& path)
  {
    return std::filesystem::weakly_canonical(
      std::filesystem::absolute(normalize_library_lookup_path(path))
    );
  }

  void
  context::load_library(const std::filesystem::path& path)
  {
    using peelo::unicode::encoding::utf8::decode;

    if (!loaded_libraries)
    {
      loaded_libraries = std::make_shared<library_storage>();
    }

    const auto canonical_key = library_lookup_key(path);

    if (loaded_libraries->loaded.contains(canonical_key))
    {
      return;
    }

    std::string last_load_error = "Unable to load dynamic library.";

    for (const auto& candidate : library_search_paths(path))
    {
      if (!std::filesystem::exists(candidate))
      {
        continue;
      }

      try
      {
        auto library = std::make_unique<dylib::library>(
          candidate.string(),
          dylib::decorations::none()
        );
        const auto init = library->get_function<void(context&)>(plugin_symbol);

        init(*this);
        loaded_libraries->loaded.insert(canonical_key);
        loaded_libraries->libraries.push_back(std::move(library));
        return;
      } catch (const dylib::symbol_error& e) {
        throw error(
          error::type::system,
          U"Unable to load `" + decode(plugin_symbol) + U"' symbol from `"
            + decode(path.string()) + U"': " + decode(e.what())
        );
      } catch (const dylib::load_error& e) {
        last_load_error = e.what();
      }
    }

    throw error(
      error::type::system,
      U"Unable to load dynamic library `" + decode(path.string()) + U"': "
        + decode(last_load_error)
    );
  }
}
