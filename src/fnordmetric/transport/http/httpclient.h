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
#ifndef _libstx_HTTPCLIENT_H
#define _libstx_HTTPCLIENT_H
#include "fnordmetric/util/uri.h"
#include "fnordmetric/transport/http/httpresponsefuture.h"
#include "fnordmetric/transport/http/httpconnectionpool.h"
#include "fnordmetric/util/thread/eventloop.h"

namespace fnordmetric {
namespace http {

class HTTPClient {
public:

  HTTPClient();
  HTTPClient(HTTPClientStats* stats);

  // deprecated
  HTTPResponse executeRequest(const HTTPRequest& req);

  Status executeRequest(const HTTPRequest& req, HTTPResponse* res);

  HTTPResponse executeRequest(
      const HTTPRequest& req,
      const InetAddr& addr);

  HTTPResponse executeRequest(
      const HTTPRequest& req,
      Function<HTTPResponseFuture* (Promise<HTTPResponse> promise)> factory);

  HTTPResponse executeRequest(
      const HTTPRequest& req,
      const InetAddr& addr,
      Function<HTTPResponseFuture* (Promise<HTTPResponse> promise)> factory);

protected:
  HTTPClientStats stats_int_;
  HTTPClientStats* stats_;
  thread::EventLoop ev_;
  std::mutex mutex_;
  net::DNSCache dns_cache_;
};

}
}
#endif
