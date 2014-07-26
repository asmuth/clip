/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <memory>
#include <fnordmetric/query/backends/csv/csvtableref.h>
#include <fnordmetric/query/backends/csv/csvinputstream.h>
#include <fnordmetric/query/tableref.h>

namespace fnordmetric {
namespace query {
namespace csv_backend {

CSVTableRef::CSVTableRef(
    std::unique_ptr<CSVInputStream>&& csv) :
    csv_(std::move(csv)) {}

int CSVTableRef::getColumnIndex(const std::string& name) {
}

void CSVTableRef::executeScan(TableScan* scan) {
}

}
}
}
