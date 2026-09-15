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

#define LASKIN_VERSION_MAJOR 10
#define LASKIN_VERSION_MINOR 0
#define LASKIN_VERSION_PATCH 1

// Helper macros for stringification.
#define LASKIN_STRINGIFY2(s) #s
#define LASKIN_STRINGIFY(s) LASKIN_STRINGIFY2(s)

#define LASKIN_VERSION_STRING \
  LASKIN_STRINGIFY(LASKIN_VERSION_MAJOR) "." \
  LASKIN_STRINGIFY(LASKIN_VERSION_MINOR) "." \
  LASKIN_STRINGIFY(LASKIN_VERSION_PATCH)

namespace laskin::version
{
  static constexpr int major = LASKIN_VERSION_MAJOR;
  static constexpr int minor = LASKIN_VERSION_MINOR;
  static constexpr int patch = LASKIN_VERSION_PATCH;
}
