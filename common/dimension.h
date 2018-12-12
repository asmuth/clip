/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include <unordered_map>
#include "common/domain.h"

namespace plotfx {

struct DimensionConfig {
  std::string key;
  DomainConfig domain;
  ColorScheme colors;
};

using DimensionMap = std::unordered_map<std::string, DimensionConfig>;

template <typename T>
using DimensionMapFn = std::function<
    ReturnCode(
        const DimensionConfig& dimension,
        const Value& val_in,
        T* val_out)>;

const DimensionConfig* dimension_find(
    const DimensionMap& map,
    const std::string& key);

void dimension_add(
    DimensionMap* map,
    const std::string& key);

DimensionMapFn<Color> dimension_map_color_continuous();
DimensionMapFn<Color> dimension_map_color_discrete();

ReturnCode dimension_resolve(
    const DataFrame& data,
    DimensionConfig* dimension);

ReturnCode dimension_resolve_all(
    const DataFrame& data,
    DimensionMap* map);

} // namespace plotfx

