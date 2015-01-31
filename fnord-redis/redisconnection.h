/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_REDIS_CONNECTION_H
#define _FNORD_REDIS_CONNECTION_H
#include <memory>
#include <vector>
#include "hiredis/hiredis.h"
#include "fnord-base/option.h"
#include "fnord-base/status.h"
#include "fnord-base/logging.h"
#include "fnord-base/net/inetaddr.h"
#include "fnord-base/thread/future.h"
#include "fnord-base/thread/taskscheduler.h"

namespace fnord {
namespace redis {

class RedisConnection {
public:

  static std::unique_ptr<RedisConnection> connect(
      const net::InetAddr& addr,
      TaskScheduler* scheduler);

  RedisConnection(const RedisConnection& other) = delete;
  RedisConnection& operator=(const RedisConnection& other) = delete;
  ~RedisConnection();

  /* KEY commands */
  Future<std::string> set(const std::string& key, const std::string& value);

  Future<Option<std::string>> get(const std::string& key);

  /* LIST commands */
  Future<Option<std::string>> lpop(const std::string& key);

  Future<std::vector<std::string>> blpop(
      const std::string& key,
      const Duration& timeout);

  Future<Option<std::string>> rpop(const std::string& key);

  Future<std::vector<std::string>> brpop(
      const std::string& key,
      const Duration& timeout);

protected:

  RedisConnection(
      const fnord::net::InetAddr& addr,
      fnord::TaskScheduler* scheduler);

  void executeCommand(
      const std::vector<std::string>& args,
      Promise<std::string> promise);

  void executeCommand(
      const std::vector<std::string>& args,
      Promise<Option<std::string>> promise);

  void executeCommand(
      const std::vector<std::string>& args,
      Promise<std::vector<std::string>> promise);

  redisContext* ctx_;
};

}
}
#endif
