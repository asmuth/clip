/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/web/webinterface.h>

namespace fnordmetric {
namespace web {

std::unique_ptr<http::HTTPHandler> WebInterface::getHandler() {
  return std::unique_ptr<http::HTTPHandler>(new WebInterface());
}

bool WebInterface::handleHTTPRequest(
    http::HTTPRequest* request,
    http::HTTPResponse* response) {
  printf("handle req!\n");

  printf("request: %s\n", request->getUrl().c_str());

  for (const auto& header : request->getHeaders()) {
    printf("..header %s => %s\n", header.first.c_str(), header.second.c_str());
  }
}

}
}
