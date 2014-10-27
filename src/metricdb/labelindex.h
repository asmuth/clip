/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_LABELINDEX_H
#define _FNORDMETRIC_METRICDB_LABELINDEX_H
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <set>

namespace fnordmetric {
namespace metricdb  {

class LabelIndex {
public:
  static const uint32_t kIndexType = 0xa0f2;

  LabelIndex();

  void addLabel(const std::string& label);
  bool hasLabel(const std::string& label) const;
  std::set<std::string> labels() const;

protected:
  std::set<std::string> labels_;
  mutable std::mutex mutex_;
};


}
}

#endif
