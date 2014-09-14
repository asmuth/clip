/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/tablerepository.h>
#include <fnordmetric/sql/importstatement.h>
#include <fnordmetric/util/uri.h>

namespace fnordmetric {
namespace query {

TableRef* TableRepository::getTableRef(const std::string& table_name) {
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
    const std::string& source_uri_raw) {
  util::URI source_uri(source_uri_raw);

  printf("URI: %s\n", source_uri.toString().c_str());
  //for (const auto& backend : backends_) {
  //}

/*
  printf("IMPORT: ");
  for (const auto tbl : tables) {
    printf("'%s', ", tbl.c_str());
  }

  printf("FROM '%s'\n", source_uri.c_str());
*/
}

void TableRepository::import(const ImportStatement& import_stmt) {
  import(import_stmt.tables(), import_stmt.source_uri());
}

}
}
