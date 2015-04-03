/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_CRATEBACKEND_H
#define _FNORDMETRIC_CRATEBACKEND_H
#include <fnord-metricdb/backends/crate/crateconnection.h>
#include <fnordmetric/sql/backends/backend.h>

#include <mutex>
#include <vector>

using fnord::metric_service::crate_backend::CrateConnection;

namespace fnordmetric {
namespace query {
namespace crate_backend {

class CrateBackend : public fnordmetric::query::Backend {
public:

  static CrateBackend* singleton();

  bool openTables(
      const std::vector<std::string>& table_names,
      const fnord::URI& source_uri,
      std::vector<std::unique_ptr<TableRef>>* target) override;

protected:
  std::vector<std::shared_ptr<CrateConnection>> connections_;
  std::mutex connections_mutex_;
};

}
}
}
#endif
