/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/net/http/httpgenerator.h>

using fnord::io::OutputStream;

namespace fnord {
namespace http {

// FIXPAUL use bufferless print
void HTTPGenerator::generate(const HTTPResponse& res, OutputStream* os) {
  os->printf(
      "%s %i %s\r\n",
      res.version().c_str(),
      res.statusCode(),
      res.statusString().c_str());

  for (const auto& header : res.headers()) {
    os->printf("%s: %s\r\n", header.first.c_str(), header.second.c_str());
  }

  os->printf("\r\n");

  const auto& body = res.body();
  if (body.size() > 0) {
    os->write(body);
  }
}

}
}
