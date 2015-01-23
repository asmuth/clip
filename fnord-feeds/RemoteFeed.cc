/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-feeds/FeedService.h"
#include "fnord-feeds/RemoteFeed.h"
#include "fnord-rpc/RPC.h"

namespace fnord {
namespace feeds {

RemoteFeed::RemoteFeed(
    const String& name,
    fnord::RPCChannel* rpc_channel,
    int batch_size /* = kDefaultBatchSize */,
    int buffer_size /* = kDefaultBufferSize */) :
    name_(name),
    rpc_channel_(rpc_channel),
    batch_size_(batch_size),
    buffer_size_(buffer_size),
    offset_(0) {}

Future<bool> RemoteFeed::appendEntry(const String& entry) {
  Promise<bool> promise;

/*
  auto rpc = fnord::mkRPC(
      &FeedService::append,
      name_,
      entry);

  //rpc->call(rpc_channel_);

  rpc->onSuccess([promise] (const decltype(rpc)::ValueType& r) mutable {
    promise.success(true);
  });

  rpc->onError([promise] (const Status& status) mutable {
    promise.failure(status);
  });
*/
  return promise.future();
}

Future<Option<FeedEntry>>
    RemoteFeed::fetchEntry(const FeedOffset& offset) {
  Promise<Option<FeedEntry>> promise;
/*
  auto rpc = fnord::mkRPC(
      &FeedService::fetch,
      name_,
      offset,
      (int) 1);

  //rpc->call(rpc_channel_);

  rpc->onSuccess([promise] (const decltype(rpc)::ValueType& r) mutable {
    if (r.result().size() == 1) {
      promise.success(Some<FeedEntry>(r.result()[0]));
    } else {
      promise.success(None<FeedEntry>());
    }
  });

  rpc->onError([promise] (const Status& status) mutable {
    promise.failure(status);
  });
*/
  return promise.future();
}

Future<Vector<FeedEntry>>
    RemoteFeed::fetchEntries(
    const FeedOffset& offset,
    int batch_size) {
  Promise<Vector<FeedEntry>> promise;

/*
  auto rpc = fnord::mkRPC(
      &FeedService::fetch,
      name_,
      offset,
      (int) batch_size);

  //rpc->call(rpc_channel_);

  rpc->onSuccess([promise] (const decltype(rpc)::ValueType& r) mutable {
    Vector<FeedEntry> entries;
    promise.success(r.result());
  });

  rpc->onError([promise] (const Status& status) mutable {
    promise.failure(status);
  });
*/
  return promise.future();
}

Future<Option<FeedEntry>>
    RemoteFeed::fetchNextEntry(const FeedEntry& entry) {
  return fetchEntry(entry.next_offset);
}

Future<Option<FeedEntry>>
    RemoteFeed::fetchFirstEntry() {
  return fetchEntry(0);
}

Future<Option<FeedEntry>>
    RemoteFeed::fetchLastEntry() {
  RAISE(kNotYetImplementedError);
}

/*
void RemoteFeed::insertDone() {
  std::unique_lock<std::mutex> lk(insert_mutex_);
  cur_insert_rpc_.reset(nullptr);

  if (insert_buf_.size() > 0) {
    // FIXPAUL: batch insert
    cur_insert_rpc_ = fnord::mkRPC(
        &FeedService::append,
        name(),
        insert_buf_.back());
    insert_buf_.pop_back();

    cur_insert_rpc_->onReady(
        std::bind(&RemoteFeed::insertDone, this));
    cur_insert_rpc_->call(rpc_channel_);
  }
}

void RemoteFeed::maybeFillBuffer() {
  if (cur_fetch_rpc_.get() != nullptr || fetch_buf_.size() >= buffer_size_) {
    return;
  }

  cur_fetch_rpc_ = fnord::mkRPC(
      &FeedService::fetch,
      name(),
      offset_,
      batch_size_);

  cur_fetch_rpc_->call(rpc_channel_);
}

void RemoteFeed::fillBuffer() {
  maybeFillBuffer();
  cur_fetch_rpc_->wait();

  for (const auto& entry : cur_fetch_rpc_->result()) {
    fetch_buf_.emplace_back(entry);
    offset_ = entry.next_offset;
  }

  cur_fetch_rpc_.reset(nullptr);
}

bool RemoteFeed::getNextEntry(String* entry) {
  std::unique_lock<std::mutex> lk(fetch_mutex_);

  if (fetch_buf_.empty()) {
    fillBuffer();
  } else {
    maybeFillBuffer();
  }

  if (fetch_buf_.empty()) {
    return false;
  }

  *entry = fetch_buf_.front().data;
  fetch_buf_.pop_front();
  return true;
}
*/

}
}
