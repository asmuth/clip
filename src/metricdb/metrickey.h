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

  static void parseStatsdFormat(
      const std::string& src,
      MetricKey* key,
      std::string* value);

  const std::string& key() const;
  const std::vector<std::pair<std::string, std::string>>& labels() const;

protected:
  std::string key_;
  std::vector<std::pair<std::string, std::string>> labels_;
};

}
}
