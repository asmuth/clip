/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/net/http/cookies.h"
#include "fnord-base/inspect.h"

namespace fnord {
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
            URI::urlEncode(std::string(split, cur))));
    begin = ++cur;
  }

  return cookies;
}

std::string Cookies::mkCookie(
    const std::string& key,
    const std::string& value,
    const DateTime& expire /* = DateTime::epoch() */,
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
} // namespace fnord
