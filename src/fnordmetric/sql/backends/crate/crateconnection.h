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
#include <fnordmetric/util/uri.h>
#include <functional>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fnordmetric/sql/svalue.h>

namespace fnordmetric {
namespace query {
namespace crate_backend {

class CrateConnection {
public:

  CrateConnection(const util::URI& uri);

  void connect(const util::URI& uri);

  std::vector<std::vector<std::string>> describeTable(const std::string& table_name);

  std::string executeQuery(
      std::string query,
      std::vector<SValue> args) const;

  void executeQuery(
      std::string query,
      std::vector<SValue> args,
      std::function<bool (const rapidjson::Value&)> row_callback) const;

  void executeQuery(
      std::string query,
      std::function<bool (const rapidjson::Value&)> row_callback) const;

private:

  const util::URI& _uri;
  std::string _url;
};

}
}
}
#endif
