/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/inspect.h>
#include "BeaconService.h"

namespace fnordmetric {

void BeaconService::updateBeacon(
    const std::string& beacon_key,
    BeaconStatus status,
    Option<String> status_text,
    Option<URI> status_url,
    Option<Duration> expect_next) {
  BeaconSample smpl;
  smpl.time = DateTime();
  smpl.status = status;
  smpl.status_text = status_text;
  smpl.status_url = status_url;
  smpl.expect_next = expect_next;

  // FIXPAUL hack ;) ;) ;)
  std::unique_lock<std::mutex> lk(beacons_mutex_);
  beacons_[beacon_key].emplace_back(smpl);
}

void BeaconService::listBeacons(
    const String& prefix,
    BeaconStatus status,
    BeaconListCallbackFn fn) const {
  std::unique_lock<std::mutex> lk(beacons_mutex_);

  for (const auto& p : beacons_) {
    if (p.second.size() == 0) {
      continue;
    }

    if (prefix.length() > 0 && !StringUtil::beginsWith(p.first, prefix)) {
      continue;
    }

    const auto& b = p.second.back();

    Option<DateTime> expect_next;
    if (!b.expect_next.isEmpty()) {
      expect_next = Some(DateTime(
          b.time.unixMicros() + b.expect_next.get().microseconds()));
    }

    if (!fn(
          p.first,
          b.status,
          b.time,
          expect_next,
          b.status_text,
          b.status_url)) {
      break;
    }
  }
}

} // namespace fnordmetric
