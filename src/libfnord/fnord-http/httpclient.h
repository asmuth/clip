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
#include "fnord-base/uri.h"
#include "fnord-http/httpresponsefuture.h"
#include "fnord-base/thread/taskscheduler.h"

namespace fnord {
namespace http {

class HTTPClient {
public:

  static Future<HTTPResponse> get(
      const std::string& uri,
      fnord::TaskScheduler* sched);

  static Future<HTTPResponse> get(
      const std::string& uri,
      const HTTPMessage::HeaderList& headers,
      fnord::TaskScheduler* sched);

  static Future<HTTPResponse> get(
      const URI& uri,
      fnord::TaskScheduler* sched);

  static Future<HTTPResponse> get(
      const URI& uri,
      const HTTPMessage::HeaderList& headers,
      fnord::TaskScheduler* sched);

  static Future<HTTPResponse> executeRequest(
      const HTTPRequest& req,
      fnord::TaskScheduler* sched);

  static Future<HTTPResponse> executeRequest(
      const HTTPRequest& req,
      const fnord::net::InetAddr& addr,
      fnord::TaskScheduler* sched);

};

}
}
#endif
