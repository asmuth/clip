/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <memory>
#include <metricd/sql/backends/csv/csvbackend.h>
#include <metricd/sql/backends/csv/csvtableref.h>
#include <metricd/sql/parser/astnode.h>
#include <metricd/sql/parser/token.h>
#include <metricd/util/inputstream.h>
#include <metricd/util/runtimeexception.h>

namespace fnordmetric {
namespace query {
namespace csv_backend {

CSVBackend* CSVBackend::singleton() {
  static CSVBackend singleton_backend;
  return &singleton_backend;
}

bool CSVBackend::openTables(
    const std::vector<std::string>& table_names,
    const util::URI& source_uri,
    std::vector<std::unique_ptr<TableRef>>* target) {
  if (source_uri.scheme() != "csv") {
    return false;
  }

  if (table_names.size() != 1) {
    RAISE(
        kRuntimeError,
        "CSVBackend can only import exactly one table per source");
  }

  char col_sep = ',';
  char row_sep = '\n';
  char quote_char = '"';
  char escape_char = '\\';
  bool headers = false;

  for (const auto& param : source_uri.queryParams()) {
    if (param.first == "headers") {
      headers = param.second == "true" || param.second == "TRUE";
      continue;
    }

    if (param.first == "row_sep") {
      if (param.second.size() != 1) {
        RAISE(
            kRuntimeError,
            "invalid parameter %s for CSVBackend: '%s', must be a single "
                "character",
            param.first.c_str(),
            param.second.c_str());
      }

      row_sep = param.second[0];
      continue;
    }

    if (param.first == "col_sep") {
      if (param.second.size() != 1) {
        RAISE(
            kRuntimeError,
            "invalid parameter %s for CSVBackend: '%s', must be a single "
                "character",
            param.first.c_str(),
            param.second.c_str());
      }

      col_sep = param.second[0];
      continue;
    }

    if (param.first == "quote_char") {
      if (param.second.size() != 1) {
        RAISE(
            kRuntimeError,
            "invalid parameter %s for CSVBackend: '%s', must be a single "
                "character",
            param.first.c_str(),
            param.second.c_str());
      }

      quote_char = param.second[0];
      continue;
    }

    if (param.first == "escape_char") {
      if (param.second.size() != 1) {
        RAISE(
            kRuntimeError,
            "invalid parameter %s for CSVBackend: '%s', must be a single "
                "character",
            param.first.c_str(),
            param.second.c_str());
      }

      escape_char = param.second[0];
      continue;
    }

    RAISE(
        kRuntimeError,
        "invalid parameter for CSVBackend: '%s'", param.first.c_str());
  }

  auto csv = CSVInputStream::openFile(
      source_uri.path(),
      col_sep,
      row_sep,
      quote_char);

  target->emplace_back(
      std::unique_ptr<TableRef>(new CSVTableRef(std::move(csv), headers)));

  return true;
}

/*$
std::unique_ptr<TableRef> CSVBackend::openTable(ASTNode* import) {
}

*/

}
}
}
