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
#include <map>
#include <vector>
#include <string>
#include "fnord-base/stdtypes.h"
#include "fnord-base/thread/taskscheduler.h"
#include "fnord-base/net/dnscache.h"
#include "fnord-http/httprequest.h"
#include "fnord-http/httpresponsefuture.h"
#include "fnord-http/httpstats.h"
#include "fnord-base/stats/statsrepository.h"

namespace fnord {
namespace http {

class HTTPConnectionPool {
public:
  HTTPConnectionPool(fnord::TaskScheduler* scheduler);

  Future<HTTPResponse> executeRequest(const HTTPRequest& req);

  Future<HTTPResponse> executeRequest(
      const HTTPRequest& req,
      const fnord::net::InetAddr& addr);

  HTTPClientStats* stats();

protected:

  void parkConnection(HTTPClientConnection* conn, net::InetAddr addr);

  void leaseConnection(
      const fnord::net::InetAddr& addr,
      Promise<HTTPResponse> promise,
      Function<void (HTTPClientConnection* conn)> callback);

  fnord::TaskScheduler* scheduler_;

  std::multimap<
      std::string,
      HTTPClientConnection*> connection_cache_;
  std::mutex connection_cache_mutex_;

  fnord::net::DNSCache dns_cache_;
  HTTPClientStats stats_;
};

}
}
#endif
