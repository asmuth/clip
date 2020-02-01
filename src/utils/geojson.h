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
#include <string>
#include <vector>
#include "return_code.h"
#include "utils/json.h"
#include "graphics/polygon.h"

namespace clip {

struct GeoJSONReader {
  std::function<ReturnCode (const Poly3*, size_t)> on_polygons;
};

// Parse a RFC7946 compliant GEOJSON file
// https://tools.ietf.org/html/rfc4180
ReturnCode geojson_read_file(
    const std::string& path,
    const GeoJSONReader& reader);

} // namespace clip

