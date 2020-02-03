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
#include "geojson.h"
#include <iostream>
#include <fstream>
#include <variant>

namespace clip {

struct GeoJSONCoord {
  double value;
  int rlevel;
};

ReturnCode geojson_read_object(const GeoJSONReader& reader, std::istream* input);
ReturnCode geojson_read_objects(const GeoJSONReader& reader, std::istream* input);

ReturnCode geojson_read_coords(
    std::istream* input,
    std::vector<GeoJSONCoord>* coords) {
  if (auto rc = json_read_array_begin(input); !rc) {
    return rc;
  }

  for (int level = 0, rlevel = 0; level >= 0; ) {
    TokenType token;
    std::string token_data;
    if (auto rc = json_parse(input, &token, &token_data); !rc) {
      return rc;
    }

    switch (token) {
      case JSON_NUMBER: {
        double value;
        try {
          value = std::stod(token_data);
        } catch (...) {
          return errorf(ERROR, "invalid number: '{}'", token_data);
        }

        coords->emplace_back(GeoJSONCoord {
          value: value,
          rlevel: rlevel
        });

        rlevel = level;
        break;
      }

      case JSON_ARRAY_BEGIN:
        ++level;
        continue;

      case JSON_ARRAY_END:
        rlevel = --level;
        continue;

      default:
        return {ERROR, "coordinates must be (nested) arrays of numbers"};
    }
  }

  return OK;
}

ReturnCode geojson_read_polygon(
    const GeoJSONReader& reader,
    const std::vector<GeoJSONCoord>& coords) {
  std::vector<PolyLine3> rings;
  for (size_t i = 0; i < coords.size(); i += 2) {
    if (coords[i].rlevel == 0) {
      rings.emplace_back();
    }

    if (i + 2 > coords.size() || coords[i + 1].rlevel != 2 || rings.empty()) {
      return {ERROR, "invalid coordinate format for 'Polygon' objects"};
    }

    if (i + 2 < coords.size() && coords[i + 2].rlevel == 2) {
      rings.back().vertices.emplace_back(
          coords[i + 0].value,
          coords[i + 2].value,
          coords[i + 2].value);
    } else {
      rings.back().vertices.emplace_back(
          coords[i + 0].value,
          coords[i + 1].value,
          0);
    }
  }

  if (rings.empty()) {
    return {ERROR, "invalid coordinate format for 'Polygon' objects"};
  }

  Poly3 poly;
  poly.boundary = rings[0];
  if (rings.size() > 1) {
    poly.holes = std::vector<PolyLine3>(rings.begin() + 1, rings.end());
  }

  if (reader.on_polygons) {
    if (auto rc = reader.on_polygons(&poly, 1); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode geojson_read_multi_polygon(
    const GeoJSONReader& reader,
    const std::vector<GeoJSONCoord>& coords) {
  std::vector<std::vector<PolyLine3>> rings;
  for (size_t i = 0; i < coords.size(); i += 2) {
    switch (coords[i].rlevel) {
      case 0:
        rings.emplace_back();
        /* fallthrough */
      case 1:
        rings.back().emplace_back();
        break;
    }

    if (i + 2 > coords.size() || coords[i + 1].rlevel != 3 || rings.empty()) {
      return {ERROR, "invalid coordinate format for 'MultiPolygon' objects"};
    }

    if (i + 2 < coords.size() && coords[i + 2].rlevel == 3) {
      rings.back().back().vertices.emplace_back(
          coords[i + 0].value,
          coords[i + 2].value,
          coords[i + 2].value);
    } else {
      rings.back().back().vertices.emplace_back(
          coords[i + 0].value,
          coords[i + 1].value,
          0);
    }
  }

  std::vector<Poly3> polys;
  for (const auto& r : rings) {
    if (r.empty()) {
      return {ERROR, "invalid coordinate format for 'MultiPolygon' objects"};
    }

    Poly3 poly;
    poly.boundary = r[0];
    if (r.size() > 1) {
      poly.holes = std::vector<PolyLine3>(r.begin() + 1, r.end());
    }

    polys.emplace_back(std::move(poly));
  }

  if (reader.on_polygons) {
    if (auto rc = reader.on_polygons(polys.data(), polys.size()); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode geojson_read_point(
    const GeoJSONReader& reader,
    const std::vector<GeoJSONCoord>& coords) {
  std::optional<vec3> point;

  if (coords.size() == 2 &&
      coords[0].rlevel == 0 &&
      coords[1].rlevel == 0) {
    point = vec3(
        coords[0].value,
        coords[1].value,
        coords[2].value);
  }

  if (coords.size() == 3 &&
      coords[0].rlevel == 0 &&
      coords[1].rlevel == 0 &&
      coords[2].rlevel == 0) {
    point = vec3(
        coords[0].value,
        coords[1].value,
        0);
  }

  if (!point) {
    return {ERROR, "invalid coordinate format for 'Point' objects"};
  }

  if (reader.on_points) {
    if (auto rc = reader.on_points(&*point, 1); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode geojson_read_multi_point(
    const GeoJSONReader& reader,
    const std::vector<GeoJSONCoord>& coords) {
  std::vector<vec3> points;
  for (size_t i = 0; i < coords.size(); i += 2) {
    if (i + 2 > coords.size() || coords[i].rlevel != 0 || coords[i + 1].rlevel != 1) {
      return {ERROR, "invalid coordinate format for 'MultiPoint' objects"};
    }

    if (i + 2 < coords.size() && coords[i + 2].rlevel == 1) {
      points.emplace_back(
          coords[i + 0].value,
          coords[i + 1].value,
          coords[i + 2].value);
    } else {
      points.emplace_back(
          coords[i + 0].value,
          coords[i + 1].value,
          0);
    }
  }

  if (points.empty()) {
    return {ERROR, "invalid coordinate format for 'MultiPoint' objects"};
  }

  if (reader.on_points) {
    if (auto rc = reader.on_points(points.data(), points.size()); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode geojson_read_line_string(
    const GeoJSONReader& reader,
    const std::vector<GeoJSONCoord>& coords) {
  PolyLine3 line;
  for (size_t i = 0; i < coords.size(); i += 2) {
    if (i + 2 > coords.size() || coords[i].rlevel != 0 || coords[i + 1].rlevel != 1) {
      return {ERROR, "invalid coordinate format for 'LineString' objects"};
    }

    if (i + 2 < coords.size() && coords[i + 2].rlevel == 1) {
      line.vertices.emplace_back(
          coords[i + 0].value,
          coords[i + 1].value,
          coords[i + 2].value);
    } else {
      line.vertices.emplace_back(
          coords[i + 0].value,
          coords[i + 1].value,
          0);
    }
  }

  if (line.vertices.empty()) {
    return {ERROR, "invalid coordinate format for 'LineString' objects"};
  }

  if (reader.on_lines) {
    if (auto rc = reader.on_lines(&line, 1); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode geojson_read_multi_line_string(
    const GeoJSONReader& reader,
    const std::vector<GeoJSONCoord>& coords) {
  std::vector<PolyLine3> lines;
  for (size_t i = 0; i < coords.size(); i += 2) {
    if (coords[i].rlevel == 0) {
      lines.emplace_back();
    }

    if (i + 2 > coords.size() || coords[i + 1].rlevel != 2 || lines.empty()) {
      return {ERROR, "invalid coordinate format for 'MultiLineString' objects"};
    }

    if (i + 2 < coords.size() && coords[i + 2].rlevel == 2) {
      lines.back().vertices.emplace_back(
          coords[i + 0].value,
          coords[i + 1].value,
          coords[i + 2].value);
    } else {
      lines.back().vertices.emplace_back(
          coords[i + 0].value,
          coords[i + 1].value,
          0);
    }
  }

  if (lines.empty()) {
    return {ERROR, "invalid coordinate format for 'MultiLineString' objects"};
  }

  if (reader.on_lines) {
    if (auto rc = reader.on_lines(lines.data(), lines.size()); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode geojson_read_object_data(
    const GeoJSONReader& reader,
    std::istream* input) {
  std::string type;
  std::vector<GeoJSONCoord> coords;

  for (TokenType token = JSON_OBJECT_BEGIN; token != JSON_OBJECT_END; ) {
    std::string token_data;
    if (auto rc = json_parse(input, &token, &token_data); !rc) {
      return rc;
    }

    switch (token) {
      case JSON_STRING:
        break;
      case JSON_OBJECT_END:
        continue;
      default:
        return {ERROR, "invalid GeoJSON"};
    }

    if (token_data == "type") {
      if (auto rc = json_read_string(input, &type); !rc) {
        return rc;
      }

      continue;
    }

    if (token_data == "features") {
      if (auto rc = geojson_read_objects(reader, input); !rc) {
        return rc;
      }

      continue;
    }

    if (token_data == "geometry") {
      if (auto rc = geojson_read_object(reader, input); !rc) {
        return rc;
      }

      continue;
    }

    if (token_data == "coordinates") {
      if (auto rc = geojson_read_coords(input, &coords); !rc) {
        return rc;
      }

      continue;
    }

    json_skip(input);
  }

  if (type == "Feature" ||
      type == "FeatureCollection" ||
      type == "GeometryCollection") {
    return OK;
  }

  if (type == "Point") {
    return geojson_read_point(reader, coords);
  }

  if (type == "MultiPoint") {
    return geojson_read_multi_point(reader, coords);
  }

  if (type == "Polygon") {
    return geojson_read_polygon(reader, coords);
  }

  if (type == "MultiPolygon") {
    return geojson_read_multi_polygon(reader, coords);
  }

  if (type == "LineString") {
    return geojson_read_multi_line_string(reader, coords);
  }

  if (type == "MultiLineString") {
    return geojson_read_multi_line_string(reader, coords);
  }

  return errorf(ERROR, "invalid object type: {}", type);
}

ReturnCode geojson_read_objects(
    const GeoJSONReader& reader,
    std::istream* input) {
  if (auto rc = json_read_array_begin(input); !rc) {
    return rc;
  }

  for (TokenType token = JSON_ARRAY_BEGIN; token != JSON_ARRAY_END; ) {
    std::string token_data;
    if (auto rc = json_parse(input, &token, &token_data); !rc) {
      return rc;
    }

    switch (token) {
      case JSON_OBJECT_BEGIN:
        if (auto rc = geojson_read_object_data(reader, input); !rc) {
          return rc;
        }

        continue;
      case JSON_ARRAY_END:
        continue;
      default:
        return {ERROR, "invalid GeoJSON"};
    }
  }

  return OK;
}

ReturnCode geojson_read_object(
    const GeoJSONReader& reader,
    std::istream* input) {
  if (auto rc = json_read_object_begin(input); !rc) {
    return rc;
  }

  if (auto rc = geojson_read_object_data(reader, input); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode geojson_read_file(
    const std::string& path,
    const GeoJSONReader& reader) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    return errorf(ERROR, "unable to open file '{}': {}", path, std::strerror(errno));
  }

  return geojson_read_object(reader, &input);
}

} // namespace clip

