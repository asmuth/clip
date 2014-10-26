/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnordmetric/metricdb/samplereader.h>
#include <fnordmetric/metricdb/tableref.h>
#include <fnordmetric/metricdb/tableheaderreader.h>
#include <fnordmetric/metricdb/tableheaderwriter.h>
#include <fnordmetric/metricdb/tokenindex.h>
#include <fnordmetric/metricdb/tokenindexwriter.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

std::unique_ptr<TableRef> TableRef::createTable(
    fnord::io::File&& file,
    const std::string& key,
    uint64_t generation,
    const std::vector<uint64_t>& parents) {
  if (env()->verbose()) {
    env()->logger()->printf(
        "DEBUG",
        "Creating new sstable for metric: '%s', generation: %" PRIu64,
        key.c_str(),
        generation);
  }

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

std::unique_ptr<TableRef> TableRef::openTable(
    const std::string filename,
    TableHeaderReader* header,
    sstable::SSTableReader* reader) {
  auto table_ref = new ReadonlyTableRef(
      filename,
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
    table_(std::move(table)),
    is_writable_(true) {}

void LiveTableRef::addSample(SampleWriter const* sample, uint64_t time) {
  table_->appendRow(&time, sizeof(time), sample->data(), sample->size()); // FIXPAUL
}

std::unique_ptr<sstable::Cursor> LiveTableRef::cursor() {
  return table_->getCursor();
}

bool LiveTableRef::isWritable() const {
  return is_writable_;
}

size_t LiveTableRef::bodySize() const {
  return table_->bodySize();
}

void LiveTableRef::importTokenIndex(TokenIndex* token_index) {
  auto cur = cursor();

  while (cur->valid()) {
    void* data;
    size_t data_size;
    cur->getData(&data, &data_size);

    SampleReader<double> sample(data, data_size, token_index);
    for (const auto& def : sample.tokenDefinitions()) {
      token_index->addToken(def.second, def.first);
    }

    if (!cur->next()) {
      break;
    }
  }
}

void LiveTableRef::finalize(TokenIndex* token_index) {
  TokenIndexWriter token_index_writer(token_index);
  table_->writeIndex(
      TokenIndex::kIndexType,
      token_index_writer.data(),
      token_index_writer.size());

  table_->finalize();
}

ReadonlyTableRef::ReadonlyTableRef(
    const std::string& filename,
    uint64_t generation,
    const std::vector<uint64_t>& parents) :
    TableRef(generation, parents) {}

void ReadonlyTableRef::addSample(SampleWriter const* sample, uint64_t time) {
  RAISE(kIllegalStateError, "table is immutable");
}

std::unique_ptr<sstable::Cursor> ReadonlyTableRef::cursor() {

}

void ReadonlyTableRef::importTokenIndex(TokenIndex* token_index) {
}

void ReadonlyTableRef::finalize(TokenIndex* token_index) {
  RAISE(kIllegalStateError, "table is immutable");
}

bool ReadonlyTableRef::isWritable() const {
  return false;
}

size_t ReadonlyTableRef::bodySize() const {
  return 0;
}

}
}

