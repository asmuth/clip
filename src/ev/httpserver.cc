/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "httpserver.h"
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace ev {

ThreadedHTTPServer::ThreadedHTTPServer(
    util::ThreadPool* thread_pool) :
    thread_pool_(thread_pool) {}

void ThreadedHTTPServer::onConnection(int fd) {
  try {
    thread_pool_->run([this, fd] () {
      handleConnection(fd);
    });
  } catch (util::RuntimeException e) {
    e.debugPrint();
  }
}

void ThreadedHTTPServer::handleConnection(int fd) {
  RAISE(util::RuntimeException, "handle conn: %i", fd);
}

}
}
