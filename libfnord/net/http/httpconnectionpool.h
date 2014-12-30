/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDM_HTTPCONNECTIONPOOL_H
#define _FNORDM_HTTPCONNECTIONPOOL_H
#include <vector>
#include <string>
#include "fnord/net/http/httprequest.h"
#include "fnord/net/http/httpresponsefuture.h"
#include "fnord/thread/taskscheduler.h"

namespace fnord {
namespace http {

class HTTPConnectionPool {
public:
  HTTPConnectionPool(fnord::thread::TaskScheduler* scheduler);

  std::unique_ptr<HTTPResponseFuture> executeRequest(const HTTPRequest& req);

  std::unique_ptr<HTTPResponseFuture> executeRequest(
      const HTTPRequest& req,
      const fnord::net::InetAddr& addr);

protected:
  fnord::thread::TaskScheduler* scheduler_;
};

}
}
#endif
