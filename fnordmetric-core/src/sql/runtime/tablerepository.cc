/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/runtime/tablerepository.h>
#include <fnordmetric/sql/runtime/importstatement.h>
#include <fnord/base/exception.h>
#include <fnord/base/uri.h>

namespace fnordmetric {
namespace query {

TableRef* TableRepository::getTableRef(const std::string& table_name) const {
  auto iter = table_refs_.find(table_name);

  if (iter == table_refs_.end()) {
    return nullptr;
  } else {
    return iter->second.get();
  }
}

void TableRepository::addTableRef(
    const std::string& table_name,
    std::unique_ptr<TableRef>&& table_ref) {
  table_refs_[table_name] = std::move(table_ref);
}

void TableRepository::import(
    const std::vector<std::string>& tables,
    const std::string& source_uri_raw,
    const std::vector<std::unique_ptr<Backend>>& backends) {
  fnord::URI source_uri(source_uri_raw);

  for (const auto& backend : backends) {
    std::vector<std::unique_ptr<TableRef>> tbl_refs;

    if (backend->openTables(tables, source_uri, &tbl_refs)) {
      if (tbl_refs.size() != tables.size()) {
        RAISE(
            kRuntimeError,
            "openTables failed for '%s'\n",
            source_uri.toString().c_str());
      }

      for (int i = 0; i < tables.size(); ++i) {
        addTableRef(tables[i], std::move(tbl_refs[i]));
      }

      return;
    }
  }

  RAISE(
      kRuntimeError,
      "no backend found for '%s'\n",
      source_uri.toString().c_str());
}

void TableRepository::import(
    const ImportStatement& import_stmt,
    const std::vector<std::unique_ptr<Backend>>& backends) {
  import(import_stmt.tables(), import_stmt.source_uri(), backends);
}

}
}
