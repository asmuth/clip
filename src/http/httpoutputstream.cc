/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/http/httpoutputstream.h>

namespace fnordmetric {
namespace http {

HTTPOutputStream::HTTPOutputStream(
    util::OutputStream* output_stream) :
    output_(output_stream) {}

void HTTPOutputStream::writeStatusLine(
    const std::string& version,
    int status) {
  output_->printf("%s %i %s\r\n", version.c_str(), status, "OK"); // FIXPAUL
}

void HTTPOutputStream::writeHeaders(
    const std::vector<std::pair<std::string, std::string>>& headers) {
  for (const auto& header : headers) {
    output_->printf("%s: %s\r\n", header.first.c_str(), header.second.c_str());
  }

  output_->printf("\r\n");
}

util::OutputStream* HTTPOutputStream::getOutputStream() const {
  return output_;
}

}
}
