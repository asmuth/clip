/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/comm/feed.h"

namespace fnord {
namespace comm {

Feed::Feed(const std::string& name) : name_(name) {}

Feed::~Feed() {}

const std::string& Feed::name() const {
  return name_;
}

FeedCache::FeedCache(FeedFactory* factory) : factory_(factory) {}

Feed* FeedCache::getFeed(const std::string& name) {
  auto iter = cache_.find(name);

  if (iter == cache_.end()) {
    auto feed = factory_->getFeed(name);
    auto feedptr = feed.get();
    cache_.emplace(name, std::move(feed));
    return feedptr;
  } else {
    return iter->second.get();
  }
}

}
}

