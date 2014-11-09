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
#include <fnordmetric/metricdb/backends/disk/labelindex.h>
#include <fnordmetric/metricdb/backends/disk/labelindexreader.h>
#include <fnordmetric/metricdb/backends/disk/labelindexwriter.h>
#include <fnordmetric/metricdb/backends/disk/samplereader.h>
#include <fnordmetric/metricdb/backends/disk/tableref.h>
#include <fnordmetric/metricdb/backends/disk/tableheaderreader.h>
#include <fnordmetric/metricdb/backends/disk/tableheaderwriter.h>
#include <fnordmetric/metricdb/backends/disk/tokenindex.h>
#include <fnordmetric/metricdb/backends/disk/tokenindexwriter.h>
#include <fnordmetric/metricdb/backends/disk/tokenindexreader.h>
#include <fnordmetric/sstable/sstablereader.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {
namespace disk_backend {

std::unique_ptr<TableRef> TableRef::openTable(const std::string filename) {
  auto file = io::File::openFile(
      filename,
      io::File::O_READ | io::File::O_WRITE);

  sstable::SSTableReader reader(file.clone());
  auto header_buf = reader.readHeader();
  TableHeaderReader header(header_buf.data(), header_buf.size());

  if (env()->verbose()) {
    env()->logger()->printf(
        "DEBUG",
        "Opening sstable: '%s'",
        filename.c_str());
  }

  if (reader.bodySize() == 0) {
    return TableRef::reopenTable(
        filename,
        header.metricKey(),
        std::move(file),
        header.generation(),
        header.parents());
  } else {
    return TableRef::openTable(
        filename,
        header.metricKey(),
        reader.bodySize(),
        header.generation(),
        header.parents());
  }
}

std::unique_ptr<TableRef> TableRef::createTable(
    const std::string& filename,
    const std::string& metric_key,
    fnord::io::File&& file,
    uint64_t generation,
    const std::vector<uint64_t>& parents) {
  if (env()->verbose()) {
    env()->logger()->printf(
        "DEBUG",
        "Creating new sstable for metric: '%s', generation: %" PRIu64,
        metric_key.c_str(),
        generation);
  }

  // build header
  TableHeaderWriter header(metric_key, generation, parents);

  // create new sstable
  sstable::IndexProvider indexes;
  auto live_sstable = sstable::SSTableWriter::create(
      std::move(file),
      std::move(indexes),
      header.data(),
      header.size());

  auto table_ref = new LiveTableRef(
      filename,
      metric_key,
      std::move(live_sstable),
      generation,
      parents);

  return std::unique_ptr<TableRef>(table_ref);
}

std::unique_ptr<TableRef> TableRef::reopenTable(
    const std::string& filename,
    const std::string& metric_key,
    fnord::io::File&& file,
    uint64_t generation,
    const std::vector<uint64_t>& parents) {
  sstable::IndexProvider indexes;

  auto table = sstable::SSTableWriter::reopen(
      std::move(file),
      std::move(indexes));

  auto table_ref = new LiveTableRef(
      filename,
      metric_key,
      std::move(table),
      generation,
      parents);

  return std::unique_ptr<TableRef>(table_ref);
}

std::unique_ptr<TableRef> TableRef::openTable(
    const std::string& filename,
    const std::string& metric_key,
    size_t body_size,
    uint64_t generation,
    const std::vector<uint64_t>& parents) {
  auto table_ref = new ReadonlyTableRef(
      filename,
      metric_key,
      body_size,
      generation,
      parents);

  return std::unique_ptr<TableRef>(table_ref);
}

TableRef::TableRef(
    const std::string& filename,
    const std::string& metric_key,
    uint64_t generation,
    const std::vector<uint64_t>& parents) :
    filename_(filename),
    metric_key_(metric_key),
    generation_(generation),
    parents_(parents) {}

const std::string& TableRef::filename() const {
  return filename_;
}

const std::string& TableRef::metricKey() const {
  return metric_key_;
}

uint64_t TableRef::generation() const {
  return generation_;
}

const std::vector<uint64_t> TableRef::parents() const {
  return parents_;
}

LiveTableRef::LiveTableRef(
    const std::string& filename,
    const std::string& metric_key,
    std::unique_ptr<sstable::SSTableWriter> table,
    uint64_t generation,
    const std::vector<uint64_t>& parents) :
    TableRef(filename, metric_key, generation, parents),
    table_(std::move(table)),
    is_writable_(true) {
  printf("create live table ref\n");
}

LiveTableRef::~LiveTableRef() {
  printf("delete live table ref\n");
}

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

void LiveTableRef::import(TokenIndex* token_index, LabelIndex* label_index) {
  auto cur = cursor();

  while (cur->valid()) {
    void* data;
    size_t data_size;
    cur->getData(&data, &data_size);

    SampleReader<double> sample(data, data_size, token_index);
    for (const auto& def : sample.tokenDefinitions()) {
      token_index->addToken(def.second, def.first);
    }

    for (const auto& label : sample.labels()) {
      label_index->addLabel(label.first);
    }

    if (!cur->next()) {
      break;
    }
  }
}

void LiveTableRef::finalize(
    TokenIndex* token_index,
    LabelIndex* label_index) {
  TokenIndexWriter token_index_writer(token_index);

  table_->writeIndex(
      TokenIndex::kIndexType,
      token_index_writer.data(),
      token_index_writer.size());

  LabelIndexWriter label_index_writer(label_index);

  table_->writeIndex(
      LabelIndex::kIndexType,
      label_index_writer.data(),
      label_index_writer.size());

  table_->finalize();
}

ReadonlyTableRef::ReadonlyTableRef(
    const std::string& filename,
    const std::string& metric_key,
    size_t body_size,
    uint64_t generation,
    const std::vector<uint64_t>& parents) :
    TableRef(filename, metric_key, generation, parents),
    body_size_(body_size) {}

ReadonlyTableRef::ReadonlyTableRef(
    const TableRef& live_table) :
    body_size_(live_table.bodySize()),
    TableRef(
        live_table.filename(),
        live_table.metricKey(),
        live_table.generation(),
        live_table.parents()) {}

void ReadonlyTableRef::addSample(SampleWriter const* sample, uint64_t time) {
  RAISE(kIllegalStateError, "table is immutable");
}

std::unique_ptr<sstable::Cursor> ReadonlyTableRef::cursor() {
  auto table = openTable();
  return table->getCursor();
}

void ReadonlyTableRef::import(
    TokenIndex* token_index,
    LabelIndex* label_index) {
  auto reader = openTable();
  auto token_index_buffer = reader->readFooter(TokenIndex::kIndexType);
  if (token_index_buffer.size() == 0) {
    if (env()->verbose()) {
      env()->logger()->printf(
          "DEBUG",
          "SStable has empty token index: '%s' (%s)",
          filename_.c_str(),
          metric_key_.c_str());
    }
  } else {
    TokenIndexReader token_index_reader(
        token_index_buffer.data(),
        token_index_buffer.size());

    token_index_reader.readIndex(token_index);
  }

  auto label_index_buffer = reader->readFooter(LabelIndex::kIndexType);
  if (label_index_buffer.size() == 0) {
    if (env()->verbose()) {
      env()->logger()->printf(
          "DEBUG",
          "SStable has empty labelindex: '%s' (%s)",
          filename_.c_str(),
          metric_key_.c_str());
    }
  } else {
    LabelIndexReader label_index_reader(
        label_index_buffer.data(),
        label_index_buffer.size());

    label_index_reader.readIndex(label_index);
  }
}

void ReadonlyTableRef::finalize(
    TokenIndex* token_index,
    LabelIndex* label_index) {
  RAISE(kIllegalStateError, "table is immutable");
}

bool ReadonlyTableRef::isWritable() const {
  return false;
}

size_t ReadonlyTableRef::bodySize() const {
  return body_size_;
}

std::unique_ptr<fnord::sstable::SSTableReader> ReadonlyTableRef::openTable() {
  if (env()->verbose()) {
    env()->logger()->printf(
        "DEBUG",
        "Opening read-only sstable: '%s'",
        filename_.c_str());
  }

  auto file = io::File::openFile(filename_, io::File::O_READ);
  return std::unique_ptr<fnord::sstable::SSTableReader>(
      new sstable::SSTableReader(std::move(file)));
}

}
}
}
