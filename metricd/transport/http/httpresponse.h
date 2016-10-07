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
#ifndef _libstx_WEB_HTTPRESPONSE_H
#define _libstx_WEB_HTTPRESPONSE_H
#include <metricd/util/time.h>
#include <metricd/transport/http/httpmessage.h>
#include <metricd/transport/http/httprequest.h>
#include <metricd/transport/http/status.h>
#include <string>

namespace fnordmetric {
namespace http {

class HTTPResponse : public HTTPMessage {
public:
  /**
   * Parse the provided http response string and return the parsed http response
   */
  static HTTPResponse parse(const std::string& str);

  HTTPResponse();

  void setStatus(int status_code, const std::string& status);
  void setStatus(const HTTPStatus& status);
  void setStatusCode(int code);
  void setStatusName(const std::string& status);

  void addCookie(
      const std::string& key,
      const std::string& value,
      const UnixTime& expire = UnixTime::epoch(),
      const std::string& path = "",
      const std::string& domain = "",
      bool secure = false,
      bool httponly = false);

  void populateFromRequest(const HTTPRequest& request);

  int statusCode() const;
  const std::string& statusName() const;

protected:
  int status_code_;
  std::string status_;
};

}
}
#endif
