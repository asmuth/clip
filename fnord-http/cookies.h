/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_HTTP_COOKIES_H
#define _FNORD_HTTP_COOKIES_H
#include <vector>
#include <string>
#include "fnord/base/datetime.h"
#include <fnord/base/uri.h>

namespace fnord {
namespace http {

class Cookies {
public:
  typedef std::vector<std::pair<std::string, std::string>> CookieList;

  static bool getCookie(
      const CookieList& cookies,
      const std::string& key,
      std::string* dst);

  static CookieList parseCookieHeader(const std::string& header_str);

  static std::string mkCookie(
      const std::string& key,
      const std::string& value,
      const DateTime& expire = DateTime::epoch(),
      const std::string& path = "",
      const std::string& domain = "",
      bool secure = false,
      bool httponly = false);
};

}
}
#endif
