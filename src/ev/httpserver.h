/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_WEB_HTTPSERVER_H
#define _FNORDMETRIC_WEB_HTTPSERVER_H
#include <fnordmetric/ev/acceptor.h>
#include <fnordmetric/util/threadpool.h>

namespace fnordmetric {
namespace ev {

class HTTPServer : public ev::Acceptor::CallbackInterface {
protected:
};

class ThreadedHTTPServer : public HTTPServer {
public:
  ThreadedHTTPServer(util::ThreadPool* thread_pool);
  void onConnection(int fd) override;

protected:
  void handleConnection(int fd);
  util::ThreadPool* thread_pool_;
};

}
}
#endif
