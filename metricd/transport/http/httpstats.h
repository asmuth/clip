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
#ifndef _libstx_HTTPSTATS_H
#define _libstx_HTTPSTATS_H

#include "metricd/util/fileutil.h"
#include "metricd/util/stdtypes.h"
#include "metricd/util/stats/counter.h"
#include "metricd/util/stats/multicounter.h"
#include "metricd/util/stats/statsrepository.h"

namespace fnordmetric {
namespace http {

struct HTTPClientStats {
  stats::Counter<uint64_t> current_connections;
  stats::Counter<uint64_t> total_connections;
  stats::MultiCounter<uint64_t, uint64_t> status_codes;
  stats::Counter<uint64_t> current_requests;
  stats::Counter<uint64_t> total_requests;
  stats::Counter<uint64_t> received_bytes;
  stats::Counter<uint64_t> sent_bytes;

  HTTPClientStats() :
      status_codes(("http_status")) {}

  void exportStats(
      const String& path_prefix = "/fnord/http/client/",
      stats::StatsRepository* stats_repo = nullptr) {

    if (stats_repo == nullptr) {
      stats_repo = stats::StatsRepository::get();
    }

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "current_connections"),
        &current_connections,
        stats::ExportMode::EXPORT_NONE);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "total_connections"),
        &total_connections,
        stats::ExportMode::EXPORT_DELTA);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "status_codes"),
        &status_codes,
        stats::ExportMode::EXPORT_DELTA);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "current_requests"),
        &current_requests,
        stats::ExportMode::EXPORT_NONE);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "total_requests"),
        &total_requests,
        stats::ExportMode::EXPORT_DELTA);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "received_bytes"),
        &received_bytes,
        stats::ExportMode::EXPORT_DELTA);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "sent_bytes"),
        &sent_bytes,
        stats::ExportMode::EXPORT_DELTA);
  }

};

struct HTTPServerStats {
  stats::Counter<uint64_t> current_connections;
  stats::Counter<uint64_t> total_connections;
  stats::MultiCounter<uint64_t, uint64_t> status_codes;
  stats::Counter<uint64_t> current_requests;
  stats::Counter<uint64_t> total_requests;
  stats::Counter<uint64_t> received_bytes;
  stats::Counter<uint64_t> sent_bytes;

  HTTPServerStats() :
      status_codes(("http_status")) {}

  void exportStats(
      const String& path_prefix = "/fnord/http/client/",
      stats::StatsRepository* stats_repo = nullptr) {

    if (stats_repo == nullptr) {
      stats_repo = stats::StatsRepository::get();
    }

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "current_connections"),
        &current_connections,
        stats::ExportMode::EXPORT_NONE);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "total_connections"),
        &total_connections,
        stats::ExportMode::EXPORT_DELTA);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "status_codes"),
        &status_codes,
        stats::ExportMode::EXPORT_DELTA);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "current_requests"),
        &current_requests,
        stats::ExportMode::EXPORT_NONE);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "total_requests"),
        &total_requests,
        stats::ExportMode::EXPORT_DELTA);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "received_bytes"),
        &received_bytes,
        stats::ExportMode::EXPORT_DELTA);

    stats_repo->exportStat(
        FileUtil::joinPaths(path_prefix, "sent_bytes"),
        &sent_bytes,
        stats::ExportMode::EXPORT_DELTA);

  }
};

}
}
#endif
