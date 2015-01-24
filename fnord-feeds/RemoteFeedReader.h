/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FEEDS_REMOTEFEEDREADER_H
#define _FNORD_FEEDS_REMOTEFEEDREADER_H
#include "fnord/base/stdtypes.h"
#include "fnord/base/option.h"
#include "fnord-rpc/RPC.h"
#include "fnord-rpc/RPCClient.h"
#include "fnord-feeds/FeedEntry.h"

namespace fnord {
namespace feeds {

class RemoteFeedReader {
public:
  static const size_t kDefaultMaxBufferSize = 8192;

  RemoteFeedReader(RPCClient* rpc_client);

  Option<FeedEntry> fetchNextEntry();
  void waitForNextEntry();

  void addSourceFeed(
      URI rpc_url,
      String feed_name,
      size_t max_buffer_size = kDefaultMaxBufferSize);

  void exportStats(
      const String& path_prefix = "/fnord/feeds/reader/",
      stats::StatsRepository* stats_repo = nullptr);

  DateTime streamTime() const;

protected:

  class SourceFeed : public RefCounted {
  public:
    URI rpc_url;
    String feed_name;
    unsigned max_buffer_size;
    Deque<FeedEntry> read_buffer;
    bool is_fetching;
    uint64_t next_offset;
    DateTime stream_time;
  };

  void maybeFillBuffer(SourceFeed* source);

  RPCClient* rpc_client_;
  RefPtrVector<SourceFeed> sources_;

  Wakeup data_available_wakeup_;
  mutable std::mutex mutex_;
  //fnord::stats::Counter<uint64_t> stat_entries_written_total_;
  //fnord::stats::Counter<uint64_t> stat_entries_written_success_;
  //fnord::stats::Counter<uint64_t> stat_entries_written_error_;
  //fnord::stats::Counter<uint64_t> stat_entries_written_retry_;
};

}
}
#endif
