/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HTTPSTATS_H
#define _FNORDMETRIC_HTTPSTATS_H

#include "fnord/base/io/fileutil.h"
#include "fnord/base/stdtypes.h"
#include "fnord/stats/counter.h"
#include "fnord/stats/statsrepository.h"

namespace fnord {
namespace http {

struct HTTPClientStats {
  stats::Counter<uint64_t> open_connections;
  stats::Counter<uint64_t, uint64_t> status_codes;
  stats::Counter<uint64_t> current_requests;
  stats::Counter<uint64_t> total_requests;
  stats::Counter<uint64_t> sent_bytes;
  stats::Counter<uint64_t> received_bytes;
  stats::Counter<uint64_t> total_bytes;

  HTTPClientStats() :
      status_codes(("http_status")) {}

  void exportStats(
      const String& path_prefix = "/fnord/http/client/",
      stats::StatsRepository* stats_repo = nullptr) {

    if (stats_repo == nullptr) {
      stats_repo = stats::StatsRepository::get();
    }

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "open_connections"),
        &open_connections,
        stats::ExportMode::EXPORT_DELTA);
  }

};

}
}
#endif
