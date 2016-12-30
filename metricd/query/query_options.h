/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <atomic>
#include <memory>
#include <string>

namespace fnordmetric {

class QueryOptions {
public:

  void addProperty(const std::string& property, const std::string& value);

  const std::string* getProperty(
      const std::string& property,
      size_t idx = 0) const;

  size_t getPropertyCount(const std::string& property) const;

  void addChild(const std::string& property, QueryOptions&& opts);

  const QueryOptions* getChild(
      const std::string& property,
      size_t idx = 0) const;

  size_t getChildCount(const std::string& property) const;

protected:
  std::map<std::string, std::string> properties_;
  std::map<std::string, QueryOptions> children_;
};

} // namespace fnordmetric

