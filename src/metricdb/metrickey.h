/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <vector>

namespace fnordmetric {
namespace metricdb {

class MetricKey {
public:

  const std::string& key() const;
  const std::vector<std::pair<std::string, std::string>>& labels() const;

  void setKey(const std::string& key);
  void addLabel(const std::string& label, const std::string& value);

protected:
  std::string key_;
  std::vector<std::pair<std::string, std::string>> labels_;
};

}
}
