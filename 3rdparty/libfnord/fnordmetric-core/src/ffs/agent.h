/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_AGENT_H
#define _FNORDMETRIC_AGENT_H

#include <stdlib.h>
#include <memory>
#include <mutex>
#include "stream.h"
#include "database/database.h"

namespace fnordmetric {
namespace query {
  class QueryTest;
}

/**
 * Agent
 */
class Agent : public IStreamRepository {
  friend class query::QueryTest;
public:

  explicit Agent(
    const std::string& name,
    std::unique_ptr<database::Database>&& database);

  Agent(const Agent& copy) = delete;
  Agent& operator=(const Agent& copy) = delete;
  Agent(Agent&& move);

  /**
   * Open a stream for writing data
   */
  template <typename... T>
  std::shared_ptr<const TypedStream<T...>> openStream(
      const std::string& name,
      //const StreamDescription& description,
      const T... fields);

  /**
   * Execute a query
   */
  std::unique_ptr<query::Query> executeQuery
      (const std::string& query_string) const;

  /**
   * Find a stream by name if a stream with that name exists. Otherwise return
   * std::shared_ptr<Stream>(nullptr)
   */
  virtual std::shared_ptr<Stream> findStreamByName(
      const std::string& stream_name) const;

protected:
  std::string name_;
  std::vector<std::shared_ptr<const Stream>> streams_;
  std::mutex streams_mutex_;
  std::unique_ptr<database::Database> database_;
};

}

#include "agent_impl.h"
#endif
