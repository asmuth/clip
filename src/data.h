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
#include "graphics/measure.h"
#include "scale.h"
#include "sexpr_conv.h"

namespace clip {

using Value = std::string;
using Series = std::vector<Value>;
using SeriesRef = std::shared_ptr<const Series>;
using SeriesMap = std::unordered_map<std::string, SeriesRef>;

struct DataGroup {
  Value key;
  std::vector<size_t> index;
};

struct DataBuffer {
  enum class Type { Number, Text };
  std::vector<std::tuple<Type, size_t>> index;
  std::vector<double> numeric;
  std::vector<std::string> text;
};

size_t databuf_len(const DataBuffer& buf);

void databuf_add(DataBuffer* buf, double v);
void databuf_add(DataBuffer* buf, const std::string& v);

void databuf_to_text(
    const DataBuffer& buf,
    std::vector<std::string>* values);

void databuf_to_numbers(
    const DataBuffer& buf,
    std::vector<double>* values);

std::vector<DataGroup> series_group(const Series& data);

size_t series_len(const Series& s);

bool series_is_numeric(const Series& s);

std::vector<double> series_to_float(const Series& s);

double value_to_float(const Value&);
Value value_from_float(double);

std::vector<Measure> series_to_sizes(
    SeriesRef series,
    const ScaleConfig& domain_config,
    const Measure& low,
    const Measure& high);

ReturnCode data_load_strings(
    const Expr* expr,
    std::vector<std::string>* values);

ReturnCode data_load_simple(
    const Expr* expr,
    DataBuffer* values);

ReturnCode data_load_polylines2(
    const std::string& path,
    std::string format,
    const Expr* ref_x,
    const Expr* ref_y,
    DataBuffer* data_x,
    DataBuffer* data_y,
    std::vector<size_t>* index);

ReturnCode data_load_polys2(
    const Expr* expr,
    std::vector<Poly2>* data);

ReturnCode data_load_points2(
    const Expr* expr,
    DataBuffer* data_x,
    DataBuffer* data_y);

ReturnCode data_load_points2(
    const std::string& path,
    std::string format,
    const Expr* ref_x,
    const Expr* ref_y,
    DataBuffer* data_x,
    DataBuffer* data_y);

ReturnCode data_load(
    const Expr* expr,
    std::vector<Measure>* values);

template <typename T>
ReturnCode data_load_as(
    const Expr* expr,
    std::function<ReturnCode (const std::string&, T*)> conv,
    std::vector<T>* dst);

ReturnCode data_to_measures(
    std::vector<std::string>& src,
    const ScaleConfig& scale,
    std::vector<Measure>* dst);

ReturnCode data_parse(
    std::vector<std::string>& src,
    const ScaleConfig& scale,
    std::vector<double>* dst);

} // namespace clip

#include "data_impl.h"

