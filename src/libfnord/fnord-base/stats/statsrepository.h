/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_STATSREPOSITORY_H
#define _FNORD_STATS_STATSREPOSITORY_H
#include "fnord-base/stdtypes.h"
#include "fnord-base/stats/stat.h"

namespace fnord {
namespace stats {

enum class ExportMode {
  EXPORT_VALUE,
  EXPORT_DELTA,
  EXPORT_NONE
};

struct ExportedStat {
  const String path;
  Stat* stat;
  const ExportMode export_mode;

  ExportedStat(
      const String& p,
      Stat* s,
      ExportMode m) :
      path(p),
      stat(s),
      export_mode(m) {}
};

class StatsRepository {
public:
  static StatsRepository* get();

  void exportStat(
      String path,
      StatRef* stat,
      ExportMode export_mode);

  void forEachStat(Function<void (const ExportedStat& stat)> fn) const;

protected:
  std::vector<ExportedStat> stats_;
  mutable std::mutex mutex_;
};

void exportStat(String path, StatRef* stat, ExportMode export_mode);

}
}
#endif
