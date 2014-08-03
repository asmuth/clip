/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEB_HTTPRESPONSE_H
#define _FNORDMETRIC_WEB_HTTPRESPONSE_H
#include <vector>
#include <string>
#include <utility>

namespace fnordmetric {
namespace http {
class HTTPOutputStream;

class HTTPResponse {
public:
  HTTPResponse();

  void setStatus(int status);
  void addHeader(const std::string& key, const std::string& value);
  void addBody(const std::string& body);

  void writeToOutputStream(HTTPOutputStream* output);

protected:
  std::string version_;
  int status_;
  std::vector<std::pair<std::string, std::string>> headers_;
  std::string body_;
};

}
}
#endif
