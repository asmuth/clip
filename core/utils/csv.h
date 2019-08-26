/**
 * This file is part of the "clip" project
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
#include <string>
#include <vector>
#include "return_code.h"

namespace clip {

using CSVData = std::vector<std::vector<std::string>>;

// Parse a RFC4180 compliant CSV file
// https://tools.ietf.org/html/rfc4180
ReturnCode csv_parse(
    std::string input,
    CSVData* output);

} // namespace clip

