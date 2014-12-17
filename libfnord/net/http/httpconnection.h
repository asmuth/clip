/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_HTTP_CONNECTION_H
#define _FNORD_HTTP_CONNECTION_H
#include <memory>
#include <vector>
#include <fnord/logging/logger.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/httphandler.h>
#include <fnord/thread/taskscheduler.h>

namespace fnord {
namespace http {

class HTTPConnection {
public:
  HTTPConnection(
      int fd,
      thread::TaskScheduler* server_scheduler,
      thread::TaskScheduler* request_scheduler);

protected:
  void read();

  int fd_;
  thread::TaskScheduler* server_scheduler_;
  thread::TaskScheduler* request_scheduler_;
};

}
}
#endif
