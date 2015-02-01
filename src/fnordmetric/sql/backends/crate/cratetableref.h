/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <fnordmetric/sql/backends/crate/crateconnection.h>
#include <fnordmetric/sql/backends/tableref.h>

namespace fnordmetric {
namespace query {
namespace crate_backend {

class CrateTableRef : public TableRef {
public:

  CrateTableRef(
      CrateConnection conn,
      const std::string& table_name);

  int getColumnIndex(const std::string& name) override;
  std::string getColumnName(int index) override;
  void executeScan(TableScan* scan) override;
  std::vector<std::string> columns() override;
  size_t count() const;

protected:
  CrateConnection conn_;
  std::string table_name_;
  std::vector<std::vector<std::string>> table_columns_;
  std::vector<std::vector<std::string*>> columns_;
};

}
}
}
