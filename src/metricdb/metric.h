/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_METRIC_H_
#define _FNORDMETRIC_METRICDB_METRIC_H_
#include <fnordmetric/io/filerepository.h>
#include <fnordmetric/metricdb/sample.h>
#include <fnordmetric/sstable/livesstable.h>
#include <fnordmetric/util/datetime.h>
#include <string>
#include <vector>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

class Metric {
public:
  Metric(const std::string& key, io::FileRepository* file_repo);

  void addSample(const Sample<double>& sample);

  void scanSamples(
      const fnord::util::DateTime& time_begin,
      const fnord::util::DateTime& time_end,
      std::function<bool (Sample<double> const*)> callback);

  const std::string& key() const;

protected:
  sstable::LiveSSTable* getLiveTable();
  void mkLiveTable();

  struct SSTableRef {
    std::string filename;
    uint64_t first_key;
    uint64_t last_key;
  };

  std::string key_;
  io::FileRepository* file_repo_;
  std::vector<SSTableRef> sstables_;
  std::unique_ptr<sstable::LiveSSTable> live_sstable_;
};

}
}
#endif
