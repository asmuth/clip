/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _libstx_WEB_HTTPREQUEST_H
#define _libstx_WEB_HTTPREQUEST_H
#include <fnordmetric/util/uri.h>
#include <fnordmetric/transport/http/httpmessage.h>
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
