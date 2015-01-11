/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_STATSDAGENT_H_
#define _FNORD_STATS_STATSDAGENT_H_
#include <thread>
#include "fnord/base/stdtypes.h"
#include "fnord/net/inetaddr.h"
#include "fnord/stats/stat.h"
#include "fnord/stats/statsrepository.h"

namespace fnord {
namespace stats {

class StatsdAgent {
public:
  static const size_t kMaxPacketSize = 1024 * 48; // 48k

  StatsdAgent(
      net::InetAddr addr,
      Duration report_interval);

  StatsdAgent(
      net::InetAddr addr,
      Duration report_interval,
      StatsRepository* stats_repo);

  void start();
  void stop();

protected:
  void report();
  void reportValue(const String& path, Stat* stat, Vector<String>* out);
  void reportDelta(const String& path, Stat* stat, Vector<String>* out);

  void sendToStatsd(const Vector<String>& lines);
  void sendToStatsd(const Buffer& packet);

  net::InetAddr addr_;
  std::atomic<bool> running_;
  std::thread thread_;
  StatsRepository* stats_repo_;
  Duration report_interval_;

  HashMap<String, double> last_values_;
};


}
}
#endif
