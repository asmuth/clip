/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "data_frame.h"
#include <iostream>

namespace plotfx {

ReturnCode column_find(
    const DataFrame& frame,
    const std::string& column_name,
    const DataColumn** column) {
  for (const auto& c : frame.columns) {
    if (c.name == column_name) {
      *column = &c;
      return OK;
    }
  }

  return ReturnCode::errorf("EARG", "column not found: $0", column_name);
}

Value data_lookup(
    const DataFrame& data,
    const std::string& key,
    size_t idx) {
  const DataColumn* column;
  if (auto rc = column_find(data, key, &column); !rc) {
    return {};
  }

  if (idx < column->data.size()) {
    return column->data[idx];
  } else {
    return {};
  }
}

size_t series_len(const Series& s) {
  return s.size();
}

bool series_is_numeric(const Series& s) {
  for (const auto& v : s) {
    if (!StringUtil::isNumber(v)) {
      size_t idx;
      try {
        std::stod(v, &idx);
      } catch (...) {
        return false;
      }

      if (idx != v.size()) {
        return false;
      }
    }
  }

  return true;
}

std::vector<double> series_to_float(const Series& s) {
  std::vector<double> sf;

  for (const auto& v : s) {
    sf.push_back(value_to_float(v));
  }

  return sf;
}

double value_to_float(const Value& v) {
  try {
    return std::stod(v);
  } catch (...) {
    return 0;
  }
}

Value value_from_float(double v) {
  return std::to_string(v);
}

std::vector<DataGroup> column_group(const DataColumn& col) {
  std::vector<DataGroup> groups;

  for (size_t idx = 0; idx < col.data.size(); ) {
    DataGroup g;
    g.key = col.data[idx];
    g.begin = idx;
    g.end = idx;

    while (g.end < col.data.size() && col.data[g.end] == col.data[g.begin]) {
      g.end = ++idx;
    }

    groups.emplace_back(g);
  }

  return groups;
}

std::vector<DataGroup> series_group(const Series& data) {
  std::vector<DataGroup> groups;

  for (size_t idx = 0; idx < data.size(); ) {
    DataGroup g;
    g.key = data[idx];
    g.begin = idx;
    g.end = idx;

    while (g.end < data.size() && data[g.end] == data[g.begin]) {
      g.end = ++idx;
    }

    groups.emplace_back(g);
  }

  return groups;
}

} // namespace plotfx

