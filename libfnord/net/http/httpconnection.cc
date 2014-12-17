/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/net/http/httpconnection.h"

namespace fnord {
namespace http {

HTTPConnection::HTTPConnection(
    int fd,
    thread::TaskScheduler* server_scheduler,
    thread::TaskScheduler* request_scheduler) :
    fd_(fd),
    server_scheduler_(server_scheduler),
    request_scheduler_(request_scheduler) {
  server_scheduler_->runOnReadable(std::bind(&HTTPConnection::read, this), fd_);
}

void HTTPConnection::read() {
}

}
}

