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
#include <fnordmetric/metricdb/tableheaderreader.h>
#include <fnordmetric/metricdb/tableheaderwriter.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

std::unique_ptr<TableRef> TableRef::createTable(
    fnord::io::File&& file,
    const std::string& key,
    uint64_t generation,
    const std::vector<uint64_t>& parents) {
  // build header
  TableHeaderWriter header(key, generation, parents);

  // create new sstable
  sstable::IndexProvider indexes;
  auto live_sstable = sstable::LiveSSTable::create(
      std::move(file),
      std::move(indexes),
      header.data(),
      header.size());

  auto table_ref = new LiveTableRef(
      std::move(live_sstable),
      generation,
      parents);

  return std::unique_ptr<TableRef>(table_ref);
}

std::unique_ptr<TableRef> TableRef::reopenTable(
    fnord::io::File&& file,
    TableHeaderReader* header) {
  sstable::IndexProvider indexes;

  auto table = sstable::LiveSSTable::reopen(
      std::move(file),
      std::move(indexes));

  auto table_ref = new LiveTableRef(
      std::move(table),
      header->generation(),
      header->parents());

  return std::unique_ptr<TableRef>(table_ref);
}

TableRef::TableRef(
    uint64_t generation,
    const std::vector<uint64_t>& parents) :
    generation_(generation),
    parents_(parents) {}

uint64_t TableRef::generation() const {
  return generation_;
}

const std::vector<uint64_t> TableRef::parents() const {
  return parents_;
}

LiveTableRef::LiveTableRef(
    std::unique_ptr<sstable::LiveSSTable> table,
    uint64_t generation,
    const std::vector<uint64_t>& parents) :
    TableRef(generation, parents),
    table_(std::move(table)) {}

void LiveTableRef::addSample(SampleWriter const* sample, uint64_t time) {
  table_->appendRow(&time, sizeof(time), sample->data(), sample->size()); // FIXPAUL
}

std::unique_ptr<sstable::Cursor> LiveTableRef::cursor() {
  return table_->getCursor();
}

}
}

