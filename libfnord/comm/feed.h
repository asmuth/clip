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

namespace fnord {
namespace comm {

class Feed {
public:
  Feed(const std::string& name);
  virtual ~Feed();

  const std::string& name() const;

  virtual void append(const std::string& entry) = 0;

  //virtual void appendEntry(const std::string& entry) = 0;
  //virtual void appendEntryAsync(
  //    const std::string& entry,
  //    std::function<void (const Status& status)> callback) = 0;
  //virtual void appendEntryAsyncUnsafe(const std::string& entry) = 0;

  virtual bool getNextEntry(std::string* entry) = 0;

  //virtual Option<std::string> maybeGetNextEntry() = 0;
  //virtual void getNextEntryAsync(
  //    std::function<void (const Option<std::string>& entry)> callback) = 0;

  /**
   * Return the current read offset of the stream
   */
  virtual std::string offset() const = 0;

  virtual void setOption(
      const std::string& optname,
      const std::string& optval) = 0;

protected:
  std::string name_;
};

class FeedFactory {
public:
  virtual ~FeedFactory() {}
  virtual std::unique_ptr<Feed> getFeed(const std::string& name) = 0;
};

class FeedCache {
public:
  FeedCache(FeedFactory* factory);
  Feed* getFeed(const std::string& name);
protected:
  FeedFactory* factory_;
  std::mutex mutex_;
  std::unordered_map<std::string, std::unique_ptr<Feed>> cache_;
};

}
}
#endif
