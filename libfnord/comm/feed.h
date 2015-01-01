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

  virtual void append(const std::string& entry) = 0;

  virtual bool getNextEntry(std::string* entry) = 0;

  const std::string& name() const;

protected:
  std::string name_;
};

class FeedFactory {
public:
  virtual ~FeedFactory() {}
  virtual std::unique_ptr<Feed> getFeed(const std::string& name) = 0;
};

}
}
#endif
