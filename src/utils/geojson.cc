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

namespace clip {

ReturnCode geojson_read_object(std::istream* input);
ReturnCode geojson_read_objects(std::istream* input);

ReturnCode geojson_read_object_data(
    std::istream* input) {
  std::string obj_type;

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
      if (auto rc = json_read_string(input, &obj_type); !rc) {
        return rc;
      }

      continue;
    }

    if (token_data == "features") {
      if (auto rc = geojson_read_objects(input); !rc) {
        return rc;
      }

      continue;
    }

    if (token_data == "geometry") {
      if (auto rc = geojson_read_object(input); !rc) {
        return rc;
      }

      continue;
    }

    json_skip(input);
  }

  std::cerr << "read obj: " << obj_type << std::endl;
  return OK;
}

ReturnCode geojson_read_objects(
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
        if (auto rc = geojson_read_object_data(input); !rc) {
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
    std::istream* input) {
  if (auto rc = json_read_object_begin(input); !rc) {
    return rc;
  }

  if (auto rc = geojson_read_object_data(input); !rc) {
    return rc;
  }

  return OK;
}

ReturnCode geojson_parse_file(
    const std::string& path) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    return errorf(ERROR, "unable to open file '{}': {}", path, std::strerror(errno));
  }

  return geojson_read_object(&input);
}

} // namespace clip

