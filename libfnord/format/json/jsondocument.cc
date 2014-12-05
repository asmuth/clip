/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <set>
#include "fnord/base/exception.h"
#include "fnord/base/stringutil.h"
#include "fnord/base/inspect.h"
#include "fnord/format/jsonflatjsonreader.h"
#include "fnord/format/json/jsondocument.h"
#include "fnord/format/json/jsonpointer.h"

namespace fnord {
namespace json {

JSONDocument::JSONDocument(
    std::string json) :
    JSONDocument(JSONInputStream(io::StringInputStream::fromString(json))) {}

JSONDocument::JSONDocument(
    std::unique_ptr<io::InputStream> json) :
    JSONDocument(JSONInputStream(std::move(json))) {}

JSONDocument::JSONDocument(
    std::unique_ptr<JSONInputStream> json) :
    JSONDocument(std::move(*json)) {}

JSONDocument::JSONDocument(JSONInputStream&& json) {
  FlatJSONReader reader(std::move(json));

  reader.read([this] (
      const JSONPointer& path,
      const std::string& value) -> bool {
    data_.emplace_back(std::make_pair(path, value));
    return true;
  });
}

bool JSONDocument::getMaybe(const JSONPointer& path, std::string* dst) const {
  // FIXPAUL!!!
  for (const auto& pair : data_) {
    if (pair.first == path) {
      *dst = pair.second;
      return true;
    }
  }

  return false;
}

std::string JSONDocument::get(const JSONPointer& path) const {
  std::string value;

  if (!getMaybe(path, &value)) {
    RAISEF(kIndexError, "no such element: $0", path.toString());
  }

  return value;
}

std::string JSONDocument::get(
    const JSONPointer& path,
    const std::string& fallback) const {
  std::string value;

  if (getMaybe(path, &value)) {
    return value;
  } else {
    return fallback;
  }
}

// FIXPAUL!!!
void JSONDocument::forEach(
    const JSONPointer& path,
    std::function<bool (const JSONPointer& path)> callback) const {
  // FIXPAUL!!!
  std::set<std::string> children;

  auto prefix = path.toString();
  StringUtil::stripTrailingSlashes(&prefix);
  prefix += "/";

  // FIXPAUL!!!
  for (const auto& pair : data_) {
    auto elem_path = pair.first.toString();
    if (StringUtil::beginsWith(elem_path, prefix)) {
      for(auto cur = elem_path.begin() + prefix.length();
          cur < elem_path.end();
          ++cur) {
        if (*cur == '/') {
          elem_path.erase(cur, elem_path.end());
          break;
        }
      }

      children.insert(elem_path);
    }
  }

  for (const auto& child: children) {
    callback(JSONPointer(child));
  }
}

} // namespace json
} // namsepace fnord

