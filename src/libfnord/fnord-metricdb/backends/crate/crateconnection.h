/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_CRATEBACKEND_CRATECONNECTION_H
#define _FNORDMETRIC_CRATEBACKEND_CRATECONNECTION_H
#include <functional>
#include <fnord-base/uri.h>
#include <fnord-json/json.h>
#include <fnordmetric/sql/svalue.h>

using namespace fnord;

namespace fnord {
namespace metric_service {
namespace crate_backend {

class CrateConnection {
public:

  CrateConnection(const URI& uri);

  void connect(const URI& uri);

  std::vector<std::vector<std::string>> describeTable(
      const std::string& table_name);

  std::string executeQuery(
      std::string query,
      std::vector<std::string> args) const;

  void executeQuery(
      std::string query,
      std::vector<std::string> args,
      std::function<bool (const json::JSONObject&)> row_callback) const;

  void executeQuery(
      std::string query,
      std::function<bool (const json::JSONObject&)> row_callback) const;

private:
  const URI& _uri;
  std::string _url;
};

}
}
}
#endif
