/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_WEB_HTTPREQUEST_H
#define _libstx_WEB_HTTPREQUEST_H
#include <metricd/util/uri.h>
#include <metricd/transport/http/httpmessage.h>
#include <string>

namespace fnordmetric {
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

  static HTTPRequest mkPost(
      const URI& uri,
      const Buffer& data);

  static HTTPRequest mkPost(
      const URI& uri,
      const Buffer& data,
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

  bool keepalive() const;
  std::vector<std::pair<std::string, std::string>> cookies() const;

protected:
  kHTTPMethod method_;
  std::string url_;
};

}
}
#endif
