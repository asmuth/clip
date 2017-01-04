/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/query/query_options.h"

namespace fnordmetric {

void QueryOptions::addProperty(
    const std::string& property,
    const std::string& value) {
  properties_.emplace(property, value);
}

const std::string* QueryOptions::getProperty(
    const std::string& property,
    size_t idx /* = 0 */) const {
  auto iter = properties_.lower_bound(property);
  if (iter != properties_.end() && iter->first == property) {
    for (size_t i = 0; i < idx; ++i) {
      ++iter;
    }

    return &iter->second;
  } else {
    return nullptr;
  }
}

size_t QueryOptions::getPropertyCount(const std::string& property) const {
  return properties_.count(property);
}

void QueryOptions::addChild(const std::string& property, QueryOptions&& opts) {
  children_.emplace(property, opts);
}

const QueryOptions* QueryOptions::getChild(
    const std::string& property,
    size_t idx /* = 0 */) const {
  auto iter = children_.lower_bound(property);
  if (iter != children_.end() && iter->first == property) {
    for (size_t i = 0; i < idx; ++i) {
      ++iter;
    }

    return &iter->second;
  } else {
    return nullptr;
  }
}

size_t QueryOptions::getChildCount(const std::string& property) const {
  return children_.count(property);
}

} // namespace fnordmetric

