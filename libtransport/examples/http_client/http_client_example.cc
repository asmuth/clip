/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <libtransport/http/v1/http_client.h>

int main() {
  libtransport::http::HTTPClient client;
  if (!client.connect("google.com", 80)) {
    std::cerr << "connect failed: " << client.getError() << std::endl;
    return 1;
  }

  auto request = libtransport::http::HTTPRequest::mkGet("/humans.txt");
  libtransport::http::HTTPResponse response;
  if (!client.executeRequest(request, &response)) {
    std::cerr << "request failed: " << client.getError() << std::endl;
    return 1;
  }

  std::cerr << response.statusCode() << " " << response.statusName() << std::endl;
  std::cerr << response.body() << std::endl;

  return 0;
}

