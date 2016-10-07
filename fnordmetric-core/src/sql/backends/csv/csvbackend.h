/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_CSVBACKEND_H
#define _FNORDMETRIC_CSVBACKEND_H
#include <memory>
#include <metricd/sql/backends/backend.h>
#include <metricd/sql/backends/csv/csvtableref.h>

namespace fnordmetric {
namespace query {
class ASTNode;
namespace csv_backend {

class CSVBackend : public fnordmetric::query::Backend {
public:

  static CSVBackend* singleton();

  bool openTables(
      const std::vector<std::string>& table_names,
      const util::URI& source_uri,
      std::vector<std::unique_ptr<TableRef>>* target) override;

};

}
}
}
#endif
