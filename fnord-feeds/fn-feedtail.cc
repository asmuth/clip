/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <unistd.h>
#include "fnord-base/io/filerepository.h"
#include "fnord-base/io/fileutil.h"
#include "fnord-base/application.h"
#include "fnord-base/logging.h"
#include "fnord-base/random.h"
#include "fnord-base/thread/eventloop.h"
#include "fnord-base/thread/threadpool.h"
#include "fnord-base/wallclock.h"
#include "fnord-rpc/ServerGroup.h"
#include "fnord-rpc/RPC.h"
#include "fnord-rpc/RPCClient.h"
#include "fnord/cli/flagparser.h"
#include "fnord/json/json.h"
#include "fnord/json/jsonrpc.h"
#include "fnord-http/httprouter.h"
#include "fnord-http/httpserver.h"
#include "fnord-feeds/FeedService.h"
#include "fnord-feeds/RemoteFeedFactory.h"
#include "fnord-feeds/RemoteFeedReader.h"
#include "fnord/stats/statsdagent.h"
#include "fnord-mdb/MDB.h"
#include "cm-common/CustomerNamespace.h"
#include "cm-logjoin/LogJoin.h"

using namespace fnord;

int main(int argc, const char** argv) {
  fnord::Application::init();
  fnord::Application::logToStderr();

  fnord::cli::FlagParser flags;

  flags.defineFlag(
      "statefile",
      fnord::cli::FlagParser::T_STRING,
      false,
      NULL,
      NULL,
      "statefile",
      "<filename>");

  flags.defineFlag(
      "batch_size",
      fnord::cli::FlagParser::T_INTEGER,
      false,
      NULL,
      "2048",
      "batch_size",
      "<num>");

  flags.defineFlag(
      "buffer_size",
      fnord::cli::FlagParser::T_INTEGER,
      false,
      NULL,
      "8192",
      "buffer_size",
      "<num>");

  flags.defineFlag(
      "commit_size",
      fnord::cli::FlagParser::T_INTEGER,
      false,
      NULL,
      "1024",
      "commit_size",
      "<num>");

  flags.defineFlag(
      "max_spread_secs",
      fnord::cli::FlagParser::T_INTEGER,
      false,
      NULL,
      "10",
      "max_spread_secs",
      "<num>");

  flags.defineFlag(
      "print_time",
      fnord::cli::FlagParser::T_SWITCH,
      false,
      NULL,
      NULL,
      "print_time",
      "");

  flags.defineFlag(
      "loglevel",
      fnord::cli::FlagParser::T_STRING,
      false,
      NULL,
      "INFO",
      "loglevel",
      "<level>");

  flags.parseArgv(argc, argv);

  Logger::get()->setMinimumLogLevel(
      strToLogLevel(flags.getString("loglevel")));

  /* start event loop */
  fnord::thread::EventLoop ev;

  auto evloop_thread = std::thread([&ev] {
    ev.run();
  });

  /* set up rpc client */
  HTTPRPCClient rpc_client(&ev);

  size_t batch_size = flags.getInt("batch_size");
  size_t buffer_size = flags.getInt("buffer_size");
  size_t commit_size = flags.getInt("commit_size");
  size_t max_spread_secs = flags.getInt("max_spread_secs");
  size_t print_time = flags.isSet("print_time");

  /* set up input feed reader */
  feeds::RemoteFeedReader feed_reader(&rpc_client);
  feed_reader.setMaxSpread(max_spread_secs * kMicrosPerSecond);

  /* get source urls */
  Vector<String> uris = flags.getArgv();
  if (flags.isSet("statefile")) {
    auto statefile = FileUtil::read(flags.getString("statefile")).toString();
    for (const auto& uri : StringUtil::split(statefile, "\n")) {
      if (uri.size() > 0) {
        uris.emplace_back(uri);
      }
    }
  }

  HashMap<String, String> feed_urls;
  for (const auto& uri_raw : uris) {
    URI uri(uri_raw);
    const auto& params = uri.queryParams();

    std::string feed;
    if (!URI::getParam(params, "feed", &feed)) {
      RAISEF(
          kIllegalArgumentError,
          "feed url missing ?feed query param: $0",
          uri_raw);
    }

    feed_urls.emplace(feed, uri_raw.substr(0, uri_raw.find("?")));

    std::string offset_str;
    uint64_t offset = 0;
    if (URI::getParam(params, "offset", &offset_str)) {
      if (offset_str == "HEAD") {
        offset = std::numeric_limits<uint64_t>::max();
      } else {
        offset = std::stoul(offset_str);
      }
    }

    feed_reader.addSourceFeed(
        uri,
        feed,
        offset,
        batch_size,
        buffer_size);
  }

  DateTime last_iter;
  uint64_t rate_limit_micros = 1 * kMicrosPerSecond;

  for (;;) {
    last_iter = WallClock::now();
    feed_reader.waitForNextEntry();

    for (int i = 0; i < commit_size; ++i) {
      auto entry = feed_reader.fetchNextEntry();

      if (entry.isEmpty()) {
        break;
      }

      if (print_time) {
        fnord::iputs("[$0] $1", entry.get().time, entry.get().data);
      } else {
        fnord::iputs("$0", entry.get().data);
      }
    }

    if (flags.isSet("statefile")) {
      auto stream_offsets = feed_reader.streamOffsets();
      Buffer statefile;

      for (const auto& soff : stream_offsets) {
        statefile.append(
            StringUtil::format(
                "$0?feed=$1&offset=$2\n",
                feed_urls[soff.first],
                soff.first,
                soff.second));
      }

      FileUtil::write(flags.getString("statefile") + "~", statefile);
      FileUtil::mv(
          flags.getString("statefile") + "~",
          flags.getString("statefile"));
    }

    auto etime = WallClock::now().unixMicros() - last_iter.unixMicros();
    if (etime < rate_limit_micros) {
      usleep(rate_limit_micros - etime);
    }
  }

  evloop_thread.join();
  return 0;
}

