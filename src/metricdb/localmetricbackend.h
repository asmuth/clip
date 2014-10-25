/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_LOCALMETRICBACKEND_H
#define _FNORDMETRIC_METRICDB_LOCALMETRICBACKEND_H
#include <fnordmetric/sql/backends/backend.h>
#include <fnordmetric/sql/backends/tableref.h>
#include <memory>
#include <mutex>
#include <vector>

namespace fnordmetric {
namespace metricdb {

class LocalMetricBackend : public fnordmetric::query::Backend {
public:

  LocalMetricBackend();

  bool openTables(
      const std::vector<std::string>& table_names,
      const util::URI& source_uri,
      std::vector<std::unique_ptr<query::TableRef>>* target) override;

};

}
}
#endif
