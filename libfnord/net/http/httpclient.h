/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HTTPCLIENT_H
#define _FNORDMETRIC_HTTPCLIENT_H
#include "fnord/base/uri.h"
#include "fnord/net/http/httpresponsefuture.h"
#include "fnord/thread/taskscheduler.h"

namespace fnord {
namespace http {

class HTTPClient {
public:

  static std::unique_ptr<HTTPResponseFuture> get(
      const std::string& uri,
      fnord::thread::TaskScheduler* sched);

  static std::unique_ptr<HTTPResponseFuture> get(
      const std::string& uri,
      const HTTPMessage::HeaderList& headers,
      fnord::thread::TaskScheduler* sched);

  static std::unique_ptr<HTTPResponseFuture> get(
      const URI& uri,
      fnord::thread::TaskScheduler* sched);

  static std::unique_ptr<HTTPResponseFuture> get(
      const URI& uri,
      const HTTPMessage::HeaderList& headers,
      fnord::thread::TaskScheduler* sched);

  static std::unique_ptr<HTTPResponseFuture> executeRequest(
      const HTTPRequest& req,
      fnord::thread::TaskScheduler* sched);

  static std::unique_ptr<HTTPResponseFuture> executeRequest(
      const HTTPRequest& req,
      const fnord::net::InetAddr& addr,
      fnord::thread::TaskScheduler* sched);

};

}
}
#endif
