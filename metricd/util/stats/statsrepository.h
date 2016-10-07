/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _STX_STATS_STATSREPOSITORY_H
#define _STX_STATS_STATSREPOSITORY_H
#include "metricd/util/stdtypes.h"
#include "metricd/util/stats/stat.h"

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
#endif
