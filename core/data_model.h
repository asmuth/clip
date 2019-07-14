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
#include <string>
#include <vector>
#include "return_code.h"

namespace fviz {

using Value = std::string;
using Series = std::vector<Value>;
using SeriesRef = std::shared_ptr<const Series>;
using SeriesMap = std::unordered_map<std::string, SeriesRef>;

struct DataGroup {
  Value key;
  std::vector<size_t> index;
};

std::vector<DataGroup> series_group(const Series& data);

size_t series_len(const Series& s);

bool series_is_numeric(const Series& s);

std::vector<double> series_to_float(const Series& s);

double value_to_float(const Value&);
Value value_from_float(double);

} // namespace fviz

