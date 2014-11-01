/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_URI_H
#define _FNORDMETRIC_UTIL_URI_H
#include <string>
#include <vector>
#include <utility>

namespace fnordmetric {
namespace util {

class URI {
public:
  typedef std::vector<std::pair<std::string, std::string>> ParamList;

  static std::string urlDecode(const std::string& str);

  URI(const std::string& uri_str);
  void parse(const std::string& uri_str);

  const std::string& scheme() const;
  const std::string& userinfo() const;
  const std::string& host() const;
  const unsigned port() const;
  const std::string& path() const;
  const std::string& query() const;
  ParamList queryParams() const;
  const std::string& fragment() const;
  std::string toString() const;

  static void parseURI(
      const std::string& uri_str,
      std::string* scheme,
      std::string* userinfo,
      std::string* host,
      unsigned* port,
      std::string* path,
      std::string* query,
      std::string* fragment);

  static void parseQueryString(
      const std::string& query,
      std::vector<std::pair<std::string, std::string>>* params);

  static bool getParam(
      const ParamList&,
      const std::string& key,
      std::string* value);

protected:
  std::string scheme_;
  std::string userinfo_;
  std::string host_;
  unsigned port_;
  std::string path_;
  std::string query_;
  std::string fragment_;
};

}
}
#endif
