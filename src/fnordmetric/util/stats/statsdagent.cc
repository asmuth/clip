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
#include <unistd.h>
#include "fnordmetric/util/logging.h"
#include "fnordmetric/util/stats/statsdagent.h"
#include "fnordmetric/util/wallclock.h"

namespace stats {

StatsdAgent::StatsdAgent(
    InetAddr addr,
    Duration report_interval) :
    StatsdAgent(addr, report_interval, StatsRepository::get()) {}

StatsdAgent::StatsdAgent(
    InetAddr addr,
    Duration report_interval,
    StatsRepository* stats_repo) :
    addr_(addr),
    stats_repo_(stats_repo),
    report_interval_(report_interval),
    running_(false) {}

StatsdAgent::~StatsdAgent() {
  if (running_) {
    stop();
  }
}

void StatsdAgent::start() {
  running_ = true;

  thread_ = std::thread([this] () {
    auto last_report = WallClock::unixMicros();

    while (running_) {
      auto next_report = last_report + report_interval_.microseconds();
      while (running_ && WallClock::unixMicros() < next_report) {
        usleep(100000);
      }

      last_report = WallClock::unixMicros();

      try {
        report();
      } catch (const StandardException& e) {
        logError("fnord.statsd_agent", e, "StatsD push failed");
      }
    }
  });
}

void StatsdAgent::stop() {
  running_ = false;
  thread_.join();
}

void StatsdAgent::report() {
  Vector<String> lines;

  stats_repo_->forEachStat([this, &lines] (const ExportedStat& stat) {
    switch (stat.export_mode) {

      case ExportMode::EXPORT_VALUE:
        reportValue(stat.path, stat.stat, &lines);
        break;

      case ExportMode::EXPORT_DELTA:
        reportDelta(stat.path, stat.stat, &lines);
        break;

      case ExportMode::EXPORT_NONE:
        break;

    }
  });

  sendToStatsd(lines);
}

void StatsdAgent::reportValue(
    const String& path,
    Stat* stat,
    Vector<String>* out) {
  BufferStatsSinkStatsSink stats_sink;
  stat->exportAll(path,&stats_sink);

  for (const auto& pair : stats_sink.values()) {
    out->emplace_back(StringUtil::format("$0:$1", pair.first, pair.second));
  }
}

void StatsdAgent::reportDelta(
    const String& path,
    Stat* stat,
    Vector<String>* out) {
  BufferStatsSinkStatsSink stats_sink;
  stat->exportAll(path,&stats_sink);

  for (const auto& pair : stats_sink.values()) {
    auto delta = pair.second - last_values_[pair.first];
    last_values_[pair.first] = pair.second;
    out->emplace_back(StringUtil::format("$0:$1", pair.first, delta));
  }
}

void StatsdAgent::sendToStatsd(const Vector<String>& lines) {
  Vector<Buffer> pkts;

  for (const auto& line : lines) {
    if (pkts.size() == 0 ||
        pkts.back().size() + line.length() + 2 >= kMaxPacketSize) {
      pkts.emplace_back();
    }

    pkts.back().append(line);
    pkts.back().append("\n");
  }

  for (const auto& pkt : pkts) {
    sendToStatsd(pkt);
  }
}

void StatsdAgent::sendToStatsd(const Buffer& packet) {
  sock_.sendTo(packet, addr_);
}

}
