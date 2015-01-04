/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/exception.h"
#include "fnord/thread/taskscheduler.h"
#include "fnord/net/redis/redisconnection.h"

namespace fnord {
namespace redis {

RedisConnection::RedisConnection(
    const fnord::net::InetAddr& addr,
    fnord::TaskScheduler* scheduler) {
  ctx_ = redisConnect(addr.ip().c_str(), addr.port());

  if (ctx_ == NULL || ctx_->err) {
    RAISEF(kIOError, "redisConnect() failed: $0", std::string(ctx_->errstr));
  }
}

std::unique_ptr<RedisConnection> RedisConnection::connect(
    const fnord::net::InetAddr& addr,
    fnord::TaskScheduler* sched) {
  return std::unique_ptr<RedisConnection>(new RedisConnection(addr, sched));
}

Future<std::string> RedisConnection::set(
    const std::string& key,
    const std::string& value) {
  std::vector<std::string> args = { "set" };
  args.emplace_back(key);
  args.emplace_back(value);

  Promise<std::string> promise;
  executeCommand(args, promise);
  return promise.future();
}

Future<Option<std::string>> RedisConnection::get(const std::string& key) {
  std::vector<std::string> args = { "get" };
  args.emplace_back(key);

  Promise<Option<std::string>> promise;
  executeCommand(args, promise);
  return promise.future();
}

Future<Option<std::string>> RedisConnection::lpop(const std::string& key) {
  std::vector<std::string> args = { "lpop" };
  args.emplace_back(key);

  Promise<Option<std::string>> promise;
  executeCommand(args, promise);
  return promise.future();
}

Future<std::vector<std::string>> RedisConnection::blpop(
    const std::string& key,
    const Duration& timeout) {
  std::vector<std::string> args = { "blpop" };
  args.emplace_back(key);
  args.emplace_back(StringUtil::toString(timeout.seconds()));

  Promise<std::vector<std::string>> promise;
  executeCommand(args, promise);
  return promise.future();
}

Future<Option<std::string>> RedisConnection::rpop(const std::string& key) {
  std::vector<std::string> args = { "rpop" };
  args.emplace_back(key);

  Promise<Option<std::string>> promise;
  executeCommand(args, promise);
  return promise.future();
}

Future<std::vector<std::string>> RedisConnection::brpop(
    const std::string& key,
    const Duration& timeout) {
  std::vector<std::string> args = { "brpop" };
  args.emplace_back(key);
  args.emplace_back(StringUtil::toString(timeout.seconds()));

  Promise<std::vector<std::string>> promise;
  executeCommand(args, promise);
  return promise.future();
}

void RedisConnection::executeCommand(
    const std::vector<std::string>& args,
    Promise<std::vector<std::string>> promise) {
  size_t argc = args.size();
  std::vector<const char*> argv;
  std::vector<size_t> argvlen;

  for (const auto& arg : args) {
    argv.push_back(arg.c_str());
    argvlen.push_back(arg.length());
  }

  auto reply = (redisReply *) redisCommandArgv(
      ctx_,
      argc,
      argv.data(),
      argvlen.data());

  switch (reply->type) {
    case REDIS_REPLY_ARRAY: {
      std::vector<std::string> elements;

      for (int i = 0; i < reply->elements; ++i) {
        auto elem = reply->element[i];

        if (elem->type == REDIS_REPLY_STRING) {
          elements.emplace_back(elem->str);
        } else {
          promise.failure(
              Status(eRuntimeError, "unexpected redis return type"));
          goto exit;
        }
      }

      promise.success(elements);
      break;
    }

    case REDIS_REPLY_ERROR:
      promise.failure(Status(eRuntimeError, reply->str));
      break;

    default:
      promise.failure(Status(eRuntimeError, "unexpected redis return type"));
      break;
  }

exit:
  freeReplyObject(reply);
}

void RedisConnection::executeCommand(
    const std::vector<std::string>& args,
    Promise<Option<std::string>> promise) {
  size_t argc = args.size();
  std::vector<const char*> argv;
  std::vector<size_t> argvlen;

  for (const auto& arg : args) {
    argv.push_back(arg.c_str());
    argvlen.push_back(arg.length());
  }

  auto reply = (redisReply *) redisCommandArgv(
      ctx_,
      argc,
      argv.data(),
      argvlen.data());

  switch (reply->type) {
    case REDIS_REPLY_STRING:
      promise.success(Some(std::string(reply->str)));
      break;

    case REDIS_REPLY_NIL:
      promise.success(None<std::string>());
      break;

    case REDIS_REPLY_ERROR:
      promise.failure(Status(eRuntimeError, reply->str));
      break;

    default:
      promise.failure(Status(eRuntimeError, "unexpected redis return type"));
      break;
  }

  freeReplyObject(reply);
}

void RedisConnection::executeCommand(
    const std::vector<std::string>& args,
    Promise<std::string> promise) {
  size_t argc = args.size();
  std::vector<const char*> argv;
  std::vector<size_t> argvlen;

  for (const auto& arg : args) {
    argv.push_back(arg.c_str());
    argvlen.push_back(arg.length());
  }

  auto reply = (redisReply *) redisCommandArgv(
      ctx_,
      argc,
      argv.data(),
      argvlen.data());

  switch (reply->type) {
    case REDIS_REPLY_STATUS:
      promise.success(reply->str);
      break;

    case REDIS_REPLY_ERROR:
      promise.failure(Status(eRuntimeError, reply->str));
      break;

    default:
      promise.failure(Status(eRuntimeError, "unexpected redis return type"));
      break;
  }

  freeReplyObject(reply);
}

RedisConnection::~RedisConnection() {
  redisFree(ctx_);
}

}
}
