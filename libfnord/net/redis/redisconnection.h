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
#include "fnord/3rdparty/hiredis/hiredis.h"
#include "fnord/base/option.h"
#include "fnord/base/status.h"
#include "fnord/logging/logger.h"
#include "fnord/net/inetaddr.h"
#include "fnord/thread/taskscheduler.h"

namespace fnord {
namespace redis {

class RedisConnection {
public:
  typedef std::function<void (const Status& status)> VoidReplyCallback;

  typedef std::function<void (
      const Status& status,
      const Option<std::string>& reply)> StringReplyCallback;

  typedef std::function<void (
      const Status& status,
      const Option<std::vector<std::string>>& reply)> ArrayReplyCallback;

  static std::unique_ptr<RedisConnection> connect(
      const fnord::net::InetAddr& addr,
      fnord::thread::TaskScheduler* scheduler);

  RedisConnection(const RedisConnection& other) = delete;
  RedisConnection& operator=(const RedisConnection& other) = delete;
  ~RedisConnection();

  /* KEY commands */
  void set(
      const std::string& key,
      const std::string& value,
      VoidReplyCallback callback);

  void get(
      const std::string& key,
      StringReplyCallback callback);

  /* LIST commands */
  void lpop(
      const std::string& key,
      StringReplyCallback callback);

  void blpop(
      const std::string& key,
      uint64_t timeout_secs,
      ArrayReplyCallback callback);

  void rpop(
      const std::string& key,
      StringReplyCallback callback);

  void brpop(
      const std::string& key,
      uint64_t timeout_secs,
      ArrayReplyCallback callback);

protected:

  RedisConnection(
      const fnord::net::InetAddr& addr,
      fnord::thread::TaskScheduler* scheduler);

  void executeCommand(
      const std::vector<std::string>& args,
      ArrayReplyCallback callback);

  void executeCommand(
      const std::vector<std::string>& args,
      StringReplyCallback callback);

  void executeCommand(
      const std::vector<std::string>& args,
      VoidReplyCallback callback);

  redisContext* ctx_;
};

}
}
#endif
