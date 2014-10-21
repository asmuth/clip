/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_METRICDB_METRICCURSOR_H
#define _FNORD_METRICDB_METRICCURSOR_H
#include <fnordmetric/metricdb/metricsnapshot.h>
#include <fnordmetric/metricdb/samplereader.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnordmetric {
namespace metricdb {

class MetricCursor {
public:
  MetricCursor(std::shared_ptr<MetricSnapshot> snapshot);
  MetricCursor(const MetricCursor& copy) = delete;
  MetricCursor& operator=(const MetricCursor& copy) = delete;

  bool next();
  bool valid();

  uint64_t time() const;
  SampleReader* sample() const;

protected:
  std::shared_ptr<MetricSnapshot> snapshot_;
  fnord::sstable::Cursor* tableCursor();
  fnord::sstable::Cursor* table_cur_;
};


}
}

#endif
