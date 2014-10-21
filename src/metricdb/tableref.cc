/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/tableref.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

TableRef::TableRef() {}

LiveTableRef::LiveTableRef(
    std::unique_ptr<sstable::LiveSSTable> table) :
    table_(std::move(table)) {}

void LiveTableRef::addSample(SampleWriter const* sample, uint64_t time) {
  //table_->appendRow(&time, sizeof(time), sample->data(), sample->size()); // FIXPAUL
}

std::unique_ptr<sstable::Cursor> LiveTableRef::cursor() {
  return table_->getCursor();
}

}
}

