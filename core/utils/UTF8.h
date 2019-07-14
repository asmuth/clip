/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <locale>
#include "stdtypes.h"

namespace fviz {

class UTF8 {
public:

  static char32_t nextCodepoint(const char** cur, const char* end);

  static void encodeCodepoint(char32_t codepoint, String* target);

  static bool isValidUTF8(const String& str);
  static bool isValidUTF8(const char* str, size_t size);

};

} // namespace fviz
