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
#include <fnord-base/uri.h>
#include <fnord/net/http/httpmessage.h>
#include <string>

namespace fnord {
namespace http {
class HTTPInputStream;

class HTTPRequest : public HTTPMessage {
public:

  static HTTPRequest mkGet(const std::string& uri);

  static HTTPRequest mkGet(
      const std::string& uri,
      const HTTPMessage::HeaderList& headers);

  static HTTPRequest mkGet(const URI& uri);

  static HTTPRequest mkGet(
      const URI& uri,
      const HTTPMessage::HeaderList& headers);

  /**
   * Parse the provided http request string and return the parsed http request
   */
  static HTTPRequest parse(const std::string& str);

  HTTPRequest();
  HTTPRequest(kHTTPMethod method, const std::string& uri);

  kHTTPMethod method() const;
  void setMethod(kHTTPMethod method);

  const std::string& uri() const;
  void setURI(const std::string& uri);

  const bool keepalive() const;
  std::vector<std::pair<std::string, std::string>> cookies() const;

protected:
  kHTTPMethod method_;
  std::string url_;
};

}
}
#endif
