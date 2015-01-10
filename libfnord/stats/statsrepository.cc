/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/stats/statsrepository.h"

namespace fnord {
namespace stats {

StatsRepository* StatsRepository::get() {
  static StatsRepository singleton;
  return &singleton;
}

void StatsRepository::exportStat(
    const String& path,
    StatRef* stat,
    ExportMode export_mode) {
  ScopedLock<std::mutex> lk(mutex_);
  stats_.emplace_back(path, stat->getStat(), export_mode);
}

}
}
