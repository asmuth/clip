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
#include <fnordmetric/sstable/livesstable.h>
#include <string>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

class TableRef {
public:
  TableRef();
  TableRef(const TableRef& other) = delete;
  TableRef& operator=(const TableRef& other) = delete;

  virtual void addSample(const Sample<double>& sample, uint64_t time) = 0;
  virtual std::unique_ptr<sstable::Cursor> cursor() = 0;

};

class LiveTableRef : public TableRef {
public:
  LiveTableRef(std::unique_ptr<sstable::LiveSSTable> table);

  void addSample(const Sample<double>& sample, uint64_t time) final;
  std::unique_ptr<sstable::Cursor> cursor() final;

protected:
  std::unique_ptr<sstable::LiveSSTable> table_;
};


}
}
#endif
