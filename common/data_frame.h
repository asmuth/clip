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
#pragma once
#include <string>
#include <vector>
#include "common/utils/return_code.h"

namespace plotfx {

using Value = std::string;
using Series = std::vector<Value>;

// FIXME: rename to column?
struct DataColumn {
  std::string name;
  Series data;
};

struct DataFrame {
  std::vector<DataColumn> columns;
};

ReturnCode column_find(
    const DataFrame& frame,
    const std::string& column_name,
    const DataColumn** column);

struct DataGroup {
  Value key;
  size_t begin;
  size_t end;
};

Value data_lookup(
    const DataFrame& data,
    const std::string& key,
    size_t idx);

std::vector<DataGroup> column_group(const DataColumn& col);

size_t series_len(const Series& s);

bool series_is_numeric(const Series& s);

std::vector<double> series_to_float(const Series& s);

double value_to_float(const Value&);
Value value_from_float(double);

} // namespace plotfx

