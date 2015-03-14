/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_BEACON_SERVICE_H
#define _FNORD_BEACON_SERVICE_H
#include <mutex>
#include <stdlib.h>
#include <string>
#include "fnord-base/datetime.h"
#include <fnord-base/duration.h>
#include <fnord-base/option.h>
#include "fnord-base/uri.h"
#include "fnord-metricdb/metricrepository.h"
#include "fnord-base/stats/counter.h"
#include "fnord-base/thread/taskscheduler.h"

namespace fnordmetric {

enum class BeaconStatus {
  HEALTHY,
  UNHEALTHY
};

class BeaconService {
public:

  /**
   * List all metrics
   */
  //std::vector<IMetric*> listBeacons() const;

  /**
   * Update a beacon
   */
  void updateBeacon(
      const std::string& beacon_key,
      BeaconStatus status,
      Option<String> status_text,
      Option<URI> status_url,
      Option<Duration> expect_next);

  /**
   * Return all samples for the provided metric and time range
   */
  //void fetchBeaconHistory(
  //    const std::string& beacon_key,
  //    const fnord::DateTime& time_begin,
  //    const fnord::DateTime& time_end,
  //    std::function<bool (Sample* sample)> callback);

protected:

  struct BeaconSample {
    BeaconStatus status;
    Option<String> status_text;
    Option<URI> status_url;
    Option<Duration> expect_next;
  };

  HashMap<String, List<BeaconSample>> beacons_;
};

} // namespace fnordmetric
#endif
