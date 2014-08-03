/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/http/httpserver.h>
#include <fnordmetric/http/httpinputstream.h>
#include <fnordmetric/http/httpoutputstream.h>
#include <fnordmetric/http/httprequest.h>
#include <fnordmetric/http/httpresponse.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace http {

void HTTPServer::addHandler(std::unique_ptr<HTTPHandler> handler) {
  handlers_.emplace_back(std::move(handler));
}

ThreadedHTTPServer::ThreadedHTTPServer(
    util::ThreadPool* thread_pool) :
    thread_pool_(thread_pool) {}

void ThreadedHTTPServer::onConnection(int fd) {
  thread_pool_->run([this, fd] () {
    handleConnection(fd);
  });
}

void ThreadedHTTPServer::handleConnection(int fd) {
  HTTPRequest request;
  HTTPResponse response;

  util::FileInputStream input_stream(fd, false);
  HTTPInputStream http_input_stream(&input_stream);
  request.readFromInputStream(&http_input_stream);

  for (const auto& handler : handlers_) {
    if (handler->handleHTTPRequest(&request, &response)) {
      break;
    }
  }

  util::FileOutputStream output_stream(fd, false);
  HTTPOutputStream http_output_stream(&output_stream);
  response.writeToOutputStream(&http_output_stream);
}

}
}
