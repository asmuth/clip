/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_AGENT_H
#define _FNORDMETRIC_AGENT_H

#include <memory>
#include "stream.h"
#include "storagebackend.h"

namespace fnordmetric {

/**
 * Agent
 */
class Agent {
public:

  Agent(
    const std::string& name,
    std::unique_ptr<IStorageBackend>&& storage_backend) :
    name_(name),
    storage_backend_(std::move(storage_backend)) {}

  Agent(const Agent& copy) = delete;
  Agent& operator=(const Agent& copy) = delete;

  template <typename... T>
  std::shared_ptr<const Stream<T...>> newStream(
      const std::string& name,
      //const StreamDescription& description,
      const T... fields) {
    StreamKey<T...> key(name, fields...);
    Schema<T...> schema(fields...);

    auto cursor = storage_backend_->getCursor(key.getKeyString());
    auto stream = std::make_shared<const Stream<T...>>(
        key,
        schema,
        std::move(cursor));

    // FIXPAUL make threadsafe!
    streams_.push_back(stream);
    return stream;
  }

protected:

  std::string name_;
  std::vector<std::shared_ptr<const IStream>> streams_;
  std::unique_ptr<IStorageBackend> storage_backend_;

};

}

#endif
