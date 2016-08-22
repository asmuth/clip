/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _FNORDM_HTTPCONNECTIONPOOL_H
#define _FNORDM_HTTPCONNECTIONPOOL_H
#include <map>
#include <vector>
#include <string>
#include "fnordmetric/util/stdtypes.h"
#include "fnordmetric/util/thread/taskscheduler.h"
#include "fnordmetric/util/net/dnscache.h"
#include "fnordmetric/transport/http/httprequest.h"
#include "fnordmetric/transport/http/httpresponsefuture.h"
#include "fnordmetric/transport/http/httpstats.h"
#include "fnordmetric/util/stats/statsrepository.h"

namespace fnordmetric {
namespace http {

class HTTPConnectionPool {
public:
  HTTPConnectionPool(
      TaskScheduler* scheduler,
      HTTPClientStats* stats);

  Future<HTTPResponse> executeRequest(const HTTPRequest& req);

  Future<HTTPResponse> executeRequest(
      const HTTPRequest& req,
      const InetAddr& addr);

  Future<HTTPResponse> executeRequest(
      const HTTPRequest& req,
      Function<HTTPResponseFuture* (Promise<HTTPResponse> promise)> factory);

  Future<HTTPResponse> executeRequest(
      const HTTPRequest& req,
      const InetAddr& addr,
      Function<HTTPResponseFuture* (Promise<HTTPResponse> promise)> factory);

  HTTPClientStats* stats();

protected:

  void parkConnection(HTTPClientConnection* conn, InetAddr addr);

  void leaseConnection(
      const InetAddr& addr,
      Promise<HTTPResponse> promise,
      Function<void (HTTPClientConnection* conn)> callback);

  TaskScheduler* scheduler_;

  std::multimap<
      std::string,
      HTTPClientConnection*> connection_cache_;
  std::mutex connection_cache_mutex_;

  net::DNSCache dns_cache_;
  HTTPClientStats* stats_;
};

}
}
#endif
