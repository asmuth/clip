/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <unistd.h>
#include "fnord/base/inspect.h"
#include "fnord/base/logging.h"
#include "fnord/stats/statsdagent.h"

namespace fnord {
namespace stats {

StatsdAgent::StatsdAgent(
    net::InetAddr addr,
    Duration report_interval) :
    StatsdAgent(addr, report_interval, StatsRepository::get()) {}

StatsdAgent::StatsdAgent(
    net::InetAddr addr,
    Duration report_interval,
    StatsRepository* stats_repo) :
    addr_(addr),
    stats_repo_(stats_repo),
    report_interval_(report_interval),
    running_(false) {}

void StatsdAgent::start() {
  running_ = true;

  thread_ = std::thread([this] () {
    while (running_) {
      usleep(report_interval_.microseconds());

      try {
        report();
      } catch (const StandardException& e) {
        fnord::logError("fnord.statsd_agent", e, "StatsD push failed");
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
}
