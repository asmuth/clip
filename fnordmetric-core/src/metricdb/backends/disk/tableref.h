/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_TABLEREF_H_
#define _FNORDMETRIC_METRICDB_TABLEREF_H_
#include <string>
#include <fnord/storage/sstable/sstablereader.h>
#include <fnord/storage/sstable/sstablewriter.h>
#include <fnordmetric/metricdb/backends/disk/samplewriter.h>
#include <fnordmetric/metricdb/sample.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {
namespace disk_backend {
class LabelIndex;
class TableHeaderReader;

class TableRef {
public:
  TableRef(const TableRef& other) = delete;
  TableRef& operator=(const TableRef& other) = delete;
  virtual ~TableRef() {}

  static std::unique_ptr<TableRef> openTable(const std::string filename);
  static std::unique_ptr<TableRef> openTableUnsafe(const std::string filename);

  static std::unique_ptr<TableRef> createTable(
      const std::string& filename,
      const std::string& metric_key,
      fnord::io::File&& file,
      uint64_t generation,
      const std::vector<uint64_t>& parents);

  static std::unique_ptr<TableRef> reopenTable(
      const std::string& filename,
      const std::string& metric_key,
      fnord::io::File&& file,
      uint64_t generation,
      const std::vector<uint64_t>& parents);

  static std::unique_ptr<TableRef> openTable(
      const std::string& filename,
      const std::string& metric_key,
      size_t body_size,
      uint64_t generation,
      const std::vector<uint64_t>& parents);

  virtual void addSample(SampleWriter const* sample, uint64_t time) = 0;
  virtual std::unique_ptr<sstable::Cursor> cursor() = 0;

  virtual void import(TokenIndex* token_index, LabelIndex* label_index) = 0;
  virtual void finalize(TokenIndex* token_index, LabelIndex* label_index) = 0;

  virtual bool isWritable() const = 0;
  virtual size_t bodySize() const = 0;

  const std::string& filename() const;
  const std::string& metricKey() const;
  uint64_t generation() const;
  const std::vector<uint64_t> parents() const;

protected:
  TableRef(
      const std::string& filename,
      const std::string& metric_key,
      uint64_t generation,
      const std::vector<uint64_t>& parents);

  std::string filename_;
  std::string metric_key_;
  uint64_t generation_;
  std::vector<uint64_t> parents_;
};

class LiveTableRef : public TableRef {
public:
  LiveTableRef(
      const std::string& filename,
      const std::string& metric_key,
      std::unique_ptr<sstable::SSTableWriter> table,
      uint64_t generation,
      const std::vector<uint64_t>& parents);
  ~LiveTableRef();
  void addSample(SampleWriter const* sample, uint64_t time) override;
  std::unique_ptr<sstable::Cursor> cursor() override;

  void import(
      TokenIndex* token_index,
      LabelIndex* label_index) override;

  void finalize(
      TokenIndex* token_index,
      LabelIndex* label_index) override;

  bool isWritable() const override;
  size_t bodySize() const override;

protected:
  bool is_writable_;
  std::unique_ptr<sstable::SSTableWriter> table_;
};

class ReadonlyTableRef : public TableRef {
public:
  explicit ReadonlyTableRef(
      const std::string& filename,
      const std::string& metric_key,
      size_t size,
      uint64_t generation,
      const std::vector<uint64_t>& parents);

  explicit ReadonlyTableRef(
      const TableRef& live_table);

  void addSample(SampleWriter const* sample, uint64_t time) override;
  std::unique_ptr<sstable::Cursor> cursor() override;

  void import(
      TokenIndex* token_index,
      LabelIndex* label_index) override;

  void finalize(
      TokenIndex* token_index,
      LabelIndex* label_index) override;

  bool isWritable() const override;
  size_t bodySize() const override;

protected:
  std::unique_ptr<fnord::sstable::SSTableReader> openTable();
  size_t body_size_;
};

}
}
}
#endif
