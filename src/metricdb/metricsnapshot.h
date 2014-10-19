/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_METRICSNAPSHOT_H_
#define _FNORDMETRIC_METRICDB_METRICSNAPSHOT_H_
#include <fnordmetric/sstable/livesstable.h>
#include <string>
#include <vector>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

class MetricSnapshot {
public:
  struct SSTableRef {
    std::string filename;
    uint64_t first_key;
    uint64_t last_key;
  };

  explicit MetricSnapshot(
      std::unique_ptr<sstable::LiveSSTable> live_sstable);

  sstable::LiveSSTable* liveTable() const;

protected:
  const std::vector<SSTableRef> sstables_;
  const std::unique_ptr<sstable::LiveSSTable> live_sstable_;
};

}
}
#endif
