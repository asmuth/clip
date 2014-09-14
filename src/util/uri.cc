/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/util/uri.h>

namespace fnordmetric {
namespace util {

URI::URI(const std::string& uri_str) : port_(0) {
  parse(uri_str);
}

void URI::parse(const std::string& uri_str) {
  const char* begin = uri_str.c_str();
  const char* end = begin + uri_str.size();

  /* scheme */
  for (const char* cur = begin; cur < end; ++cur) {
    if (cur[0] == ':') {
      scheme_ = std::string(begin, cur - begin);
      begin = cur + 1;
      break;
    }
  }

  if (scheme_.size() == 0) {
    RAISE(util::RuntimeException, "invalid URI: must begin with scheme:");
  }

  /* authority */
  if (begin < end - 2 && begin[0] == '/' && begin[1] == '/') {
    begin += 2;
    const char* cur = begin;
    for (; cur < end && *cur != '/' && *cur != '?' && *cur != '#'; ++cur);
    if (cur > begin) {
      parseAuthority(std::string(begin, cur - begin).c_str());
    }
  }

}

// FIXPAUL parse userinfo
void URI::parseAuthority(const std::string& authority) {
  const char* begin = authority.c_str();
  const char* end = begin + authority.size();

  /* userinfo */
  for (const char* cur = begin; cur < end; ++cur) {
    if (*cur == '/' || *cur == '?' || *cur == '#') {
      break;
    }

    if (*cur == '@') {
      userinfo_ = std::string(begin, cur - begin);
      begin = cur + 1;
      break;
    }
  }

  /* host */
  const char* cur = begin;
  for (; cur < end &&
        *cur != '/' &&
        *cur != '?' &&
        *cur != '#' &&
        *cur != ':'; ++cur);
  host_ = std::string(begin, cur - begin);

  /* port */
  if (cur < end - 1 && *cur == ':') {
    begin = ++cur;
    for (; *cur >= '0' && *cur <= '9'; ++cur);
    if (cur > begin) {
      port_ = std::stoi(std::string(begin, cur - begin));
    }
  }
}

const std::string& URI::scheme() const {
  return scheme_;
}

const std::string& URI::userinfo() const {
  return userinfo_;
}

const std::string& URI::host() const {
  return host_;
}

const unsigned URI::port() const {
  return port_;
}

std::string URI::toString() const {
  std::string uri_str;

  uri_str.append(scheme());
  uri_str.append(":");

  return uri_str;
}

}
}
