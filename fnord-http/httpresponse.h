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
#include <fnord-base/datetime.h>
#include <fnord/net/http/httpmessage.h>
#include <fnord/net/http/httprequest.h>
#include <fnord/net/http/status.h>
#include <string>

namespace fnord {
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
      const DateTime& expire = DateTime::epoch(),
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
