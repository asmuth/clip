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
    std::unique_ptr<database::Database>&& database) :
    name_(name),
    database_(std::move(database)) {}

template <typename... T>
std::shared_ptr<const TypedStream<T...>> Agent::openStream(
    const std::string& name,
    //const StreamDescription& description,
    const T... fields) {
  TypedSchema<T...> schema(fields...);
  TypedStreamKey<T...> key(name, fields...);

  streams_mutex_.lock();
  auto stream_ref = database_->openStream(key.getKeyString());
  auto stream = std::make_shared<const TypedStream<T...>>(
      key,
      schema,
      std::move(stream_ref));

  streams_.push_back(stream);
  streams_mutex_.unlock();

  return stream;
}

}
#endif
