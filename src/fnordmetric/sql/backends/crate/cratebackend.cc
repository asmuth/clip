/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnordmetric/sql/backends/crate/cratebackend.h>
#include <fnordmetric/sql/backends/crate/cratetableref.h>
#include <fnordmetric/sql/runtime/tablescan.h>
#include <iostream>

namespace fnordmetric {
namespace query {
namespace crate_backend {

CrateBackend* CrateBackend::singleton() {
    static CrateBackend singleton_backend;
    return &singleton_backend;
}

bool CrateBackend::openTables(
    const std::vector<std::string>& table_names,
    const fnord::URI& source_uri,
    std::vector<std::unique_ptr<TableRef>>* target) {
    CrateConnection conn = CrateConnection(source_uri);
    for (const auto& tbl : table_names) {
        target->emplace_back(new CrateTableRef(conn, tbl));
    }
    return true;
}

}
}
}
