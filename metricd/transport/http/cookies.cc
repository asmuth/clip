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
#include "fnordmetric/transport/http/cookies.h"
#include "fnordmetric/util/inspect.h"

namespace fnordmetric {
namespace http {

bool Cookies::getCookie(
    const CookieList& cookies,
    const std::string& key,
    std::string* dst) {
  for (const auto& cookie : cookies) {
    if (cookie.first == key) {
      *dst = cookie.second;
      return true;
    }
  }

  return false;
}

Cookies::CookieList Cookies::parseCookieHeader(const std::string& header_str) {
  std::vector<std::pair<std::string, std::string>> cookies;

  auto begin = header_str.c_str();
  auto end = begin + header_str.length();
  auto cur = begin;
  while (begin < end) {
    for (; begin < end && *begin == ' '; ++begin);
    for (; cur < end && *cur != '='; ++cur);
    auto split = ++cur;
    for (; cur < end && *cur != ';'; ++cur);
    cookies.emplace_back(
        std::make_pair(
            URI::urlDecode(std::string(begin, split - 1)),
            URI::urlDecode(std::string(split, cur))));
    begin = ++cur;
  }

  return cookies;
}

std::string Cookies::mkCookie(
    const std::string& key,
    const std::string& value,
    const UnixTime& expire /* = UnixTime::epoch() */,
    const std::string& path /* = "" */,
    const std::string& domain /* = "" */,
    bool secure /* = false */,
    bool httponly /* = false */) {
  auto cookie_str = StringUtil::format(
      "$0=$1",
      URI::urlEncode(key),
      URI::urlEncode(value));

  if (path.length() > 0) {
    cookie_str.append(StringUtil::format("; Path=$0", path));
  }

  if (domain.length() > 0) {
    cookie_str.append(StringUtil::format("; Domain=$0", domain));
  }

  if (static_cast<uint64_t>(expire) > 0) {
    cookie_str.append(StringUtil::format("; Expires=$0",
        expire.toString("%a, %d-%b-%Y %H:%M:%S UTC")));
  }

  if (httponly) {
    cookie_str.append("; HttpOnly");
  }

  if (secure) {
    cookie_str.append("; Secure");
  }

  return cookie_str;
}

} // namespace http
} // namespace fnordmetric
