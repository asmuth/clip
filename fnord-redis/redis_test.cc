/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fnord-base/net/inetaddr.h"
#include "fnord-base/net/redis/redisconnection.h"
#include "fnord/test/unittest.h"
#include "fnord-base/thread/threadpool.h"

using namespace fnord;

UNIT_TEST(RedisTest);

TEST_CASE(RedisTest, TestSimpleGetSet, [] () {
  thread::ThreadPool thread_pool;

  auto redis = redis::RedisConnection::connect(
      net::InetAddr::resolve("127.0.0.1:6379"),
      &thread_pool);

  redis->set("__libfnord_redis_test1", "bar123");
  auto res = redis->get("__libfnord_redis_test1");
  res.wait();

  EXPECT_EQ(res.get().get(), "bar123");
});



