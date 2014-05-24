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
    std::unique_ptr<IStorageBackend>&& storage_backend);

  Agent(const Agent& copy) = delete;
  Agent& operator=(const Agent& copy) = delete;

  template <typename... T>
  std::shared_ptr<const Stream<T...>> newStream(
      const std::string& name,
      //const StreamDescription& description,
      const T... fields);

protected:
  std::string name_;
  std::vector<std::shared_ptr<const IStream>> streams_;
  std::unique_ptr<IStorageBackend> storage_backend_;
};

}

#include "agent_impl.h"
#endif
