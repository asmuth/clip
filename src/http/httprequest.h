/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEB_HTTPREQUEST_H
#define _FNORDMETRIC_WEB_HTTPREQUEST_H
#include <vector>
#include <string>
#include <utility>

namespace fnordmetric {
namespace http {
class HTTPInputStream;

class HTTPRequest {
public:
  //HTTPRequest();
  void readFromInputStream(HTTPInputStream* input);

  const std::string& getUrl() const;
  const std::vector<std::pair<std::string, std::string>>& getHeaders() const;

protected:
  std::string method_;
  std::string url_;
  std::string version_;
  std::vector<std::pair<std::string, std::string>> headers_;
};

}
}
#endif
