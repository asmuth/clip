/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_TABLEREPOSITORY_H
#define _FNORDMETRIC_QUERY_TABLEREPOSITORY_H
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <fnordmetric/sql/backend.h>
#include <fnordmetric/sql/tableref.h>

namespace fnordmetric {
namespace query {
class ImportStatement;

class TableRepository {
public:

  TableRef* getTableRef(const std::string& table_name);

  void addTableRef(
      const std::string& table_name,
      std::unique_ptr<TableRef>&& table_ref);

  void addBackend(Backend* backend);

  void import(
      const std::vector<std::string>& tables,
      const std::string& source_uri);

  void import(const ImportStatement& import_stmt);

protected:
  std::vector<Backend*> backends_;
  std::unordered_map<std::string, std::unique_ptr<TableRef>> table_refs_;
};

}
}
#endif
