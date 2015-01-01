/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_LOGSTREAM_SERVICE_FEED_H
#define _FNORD_LOGSTREAM_SERVICE_FEED_H
#include <deque>
#include "fnord/comm/feed.h"
#include "fnord/comm/rpc.h"
#include "fnord/service/logstream/logstreamentry.h"

namespace fnord {
namespace logstream_service {

class LogStreamServiceFeed : public fnord::comm::Feed {
public:
  static const int kDefaultBatchSize = 1024;

  LogStreamServiceFeed(
      const std::string& name,
      fnord::comm::RPCChannel* rpc_channel,
      int batch_size = kDefaultBatchSize);

  void append(const std::string& entry) override;
  bool getNextEntry(std::string* entry) override;

  void setOption(const std::string& optname,const std::string& optval) override;

protected:
  void fillBuffer();

  fnord::comm::RPCChannel* rpc_channel_;
  int batch_size_;
  uint64_t offset_;
  std::deque<LogStreamEntry> buf_;
};

}
}
#endif
