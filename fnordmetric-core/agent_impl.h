/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_AGENT_IMPL_H
#define _FNORDMETRIC_AGENT_IMPL_H

namespace fnordmetric {

Agent::Agent(
    const std::string& name,
    std::unique_ptr<IStorageBackend>&& storage_backend) :
    name_(name),
    storage_backend_(std::move(storage_backend)) {}

template <typename... T>
std::shared_ptr<const Stream<T...>> Agent::newStream(
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

}
#endif
