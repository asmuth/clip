/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/service/metric/backends/disk/labelindex.h>

namespace fnord {
namespace metric_service {
namespace disk_backend {

LabelIndex::LabelIndex() {}

void LabelIndex::addLabel(const std::string& label) {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  labels_.insert(label);
}

bool LabelIndex::hasLabel(const std::string& label) const {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  return labels_.find(label) != labels_.end();
}

std::set<std::string> LabelIndex::labels() const {
  std::lock_guard<std::mutex> lock_holder(mutex_);
  return labels_;
}

}
}
}

