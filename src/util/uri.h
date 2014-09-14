/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_URI_H
#define _FNORDMETRIC_UTIL_URI_H
#include <string>

namespace fnordmetric {
namespace util {

class URI {
public:

  URI(const std::string& uri_str);
  void parse(const std::string& uri_str);

  const std::string& scheme() const;
  const std::string& userinfo() const;
  const std::string& host() const;
  const unsigned port() const;
  const std::string& path() const;
  const std::string& query() const;
  const std::string& fragment() const;

  std::string toString() const;

protected:
  void parseAuthority(const std::string& authority);

  std::string scheme_;
  std::string userinfo_;
  std::string host_;
  std::string path_;
  std::string query_;
  std::string fragment_;
  unsigned port_;
};

}
}
#endif
