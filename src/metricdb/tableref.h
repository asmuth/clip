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
#include <fnordmetric/metricdb/sample.h>
#include <fnordmetric/metricdb/samplewriter.h>
#include <fnordmetric/sstable/livesstable.h>
#include <string>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {
class TableHeaderReader;

class TableRef {
public:
  TableRef(uint64_t generation, const std::vector<uint64_t>& parents);
  TableRef(const TableRef& other) = delete;
  TableRef& operator=(const TableRef& other) = delete;

  static std::unique_ptr<TableRef> createTable(
      fnord::io::File&& file,
      const std::string& key,
      uint64_t generation,
      const std::vector<uint64_t>& parents);

  static std::unique_ptr<TableRef> reopenTable(
      fnord::io::File&& file,
      TableHeaderReader* header);

  static std::unique_ptr<TableRef> openTable(fnord::io::File&& file);

  virtual void addSample(SampleWriter const* sample, uint64_t time) = 0;
  virtual std::unique_ptr<sstable::Cursor> cursor() = 0;
  virtual void importTokenIndex(TokenIndex* token_index) = 0;

  virtual bool isWritable() const = 0;
  virtual size_t bodySize() const = 0;

  uint64_t generation() const;
  const std::vector<uint64_t> parents() const;

protected:
  uint64_t generation_;
  std::vector<uint64_t> parents_;
};

class LiveTableRef : public TableRef {
public:
  LiveTableRef(
      std::unique_ptr<sstable::LiveSSTable> table,
      uint64_t generation,
      const std::vector<uint64_t>& parents);

  void addSample(SampleWriter const* sample, uint64_t time) override;
  std::unique_ptr<sstable::Cursor> cursor() override;

  void importTokenIndex(TokenIndex* token_index) override;

  bool isWritable() const override;
  size_t bodySize() const override;

protected:
  std::unique_ptr<sstable::LiveSSTable> table_;
};


}
}
#endif
