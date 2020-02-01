/**
 * This file is part of the "clip" project
 *   Copyright (c) 2020 Paul Asmuth
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
#include <functional>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include "return_code.h"

namespace clip {

enum TokenType {
  JSON_OBJECT_BEGIN,
  JSON_OBJECT_END,
  JSON_ARRAY_BEGIN,
  JSON_ARRAY_END,
  JSON_STRING,
  JSON_NUMBER,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NULL
};

ReturnCode json_parse(
    std::istream* input,
    TokenType* token_type,
    std::string* token_data);

ReturnCode json_read_array_begin(
    std::istream* input);

ReturnCode json_read_object_begin(
    std::istream* input);

ReturnCode json_read_string(
    std::istream* input,
    std::string* value);

ReturnCode json_skip(std::istream* input);

} // namespace clip

