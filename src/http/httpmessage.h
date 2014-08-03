/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEB_HTTPMESSAGE_H
#define _FNORDMETRIC_WEB_HTTPMESSAGE_H
#include <vector>
#include <string>
#include <utility>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>

namespace fnordmetric {
namespace http {

class HTTPMessage {
public:
  const std::string& getVersion() const;
  void setVersion(const std::string& version);

  const std::vector<std::pair<std::string, std::string>>& getHeaders() const;
  const std::string& getHeader(const std::string& key) const;
  void addHeader(const std::string& key, const std::string& value);

  const std::string& getBody() const;
  void addBody(const std::string& body);

  std::unique_ptr<util::InputStream> getBodyInputStream() const;
  std::unique_ptr<util::OutputStream> getBodyOutputStream();

protected:
  std::string version_;
  static std::string kEmptyHeader;
  std::vector<std::pair<std::string, std::string>> headers_;
  std::string body_;
};

}
}
#endif
