/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <vector>
#include <string>
#include <utility>
#include <fnordmetric/ev/httpserver.h>
#include <fnordmetric/ev/httpinputstream.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace ev {

ThreadedHTTPServer::ThreadedHTTPServer(
    util::ThreadPool* thread_pool) :
    thread_pool_(thread_pool) {}

void ThreadedHTTPServer::onConnection(int fd) {
  thread_pool_->run([this, fd] () {
    handleConnection(fd);
  });
}

void ThreadedHTTPServer::handleConnection(int fd) {
  util::FileInputStream input_stream(fd, false);
  util::FileOutputStream output_stream(fd, false);

  /* read http headers */
  ev::HTTPInputStream http_stream(&input_stream);

  std::string method;
  std::string url;
  std::string version;
  http_stream.readStatusLine(&method, &url, &version);

  std::vector<std::pair<std::string, std::string>> headers;
  http_stream.readHeaders(&headers);

  //HTTPRequest req(method, url, version, headers);
  //HTTPResponse res;

  output_stream.printf("200 OK\nContent-Length: 5\n\nfnord\n");
}

}
}
