/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_COMM_FEED_H
#define _FNORD_COMM_FEED_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord/base/option.h"
#include "fnord/base/stdtypes.h"
#include "fnord/base/thread/future.h"

namespace fnord {
namespace comm {

class Feed {
public:
  using FeedOffset = String;

  struct FeedEntry {
    FeedOffset offset;
    FeedOffset next_offset;
    String entry_data;
  };

  Feed(const String& name);
  virtual ~Feed();

  const String& name() const;

  virtual Future<bool> appendEntry(const String& entry_data) = 0;

  virtual Future<Option<FeedEntry>> fetchEntry(const FeedOffset& offset) = 0;
  virtual Future<Option<FeedEntry>> fetchNextEntry(const FeedEntry& entry) = 0;
  virtual Future<Option<FeedEntry>> fetchFirstEntry() = 0;
  virtual Future<Option<FeedEntry>> fetchLastEntry() = 0;

  virtual void setOption(
      const String& optname,
      const String& optval) = 0;

protected:
  String name_;
};

class FeedFactory {
public:
  virtual ~FeedFactory() {}
  virtual std::unique_ptr<Feed> getFeed(const String& name) = 0;
};

class FeedCache {
public:
  FeedCache(FeedFactory* factory);
  Feed* getFeed(const String& name);
protected:
  FeedFactory* factory_;
  std::mutex mutex_;
  HashMap<String, std::unique_ptr<Feed>> cache_;
};

}
}
#endif
