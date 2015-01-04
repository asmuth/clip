/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/sstable/sstablerepair.h>
#include <fnord/thread/task.h>
#include <fnord/service/metric/backends/disk/metricrepository.h>

namespace fnord {
namespace metric_service {
namespace disk_backend {

MetricRepository::MetricRepository(
    const std::string data_dir,
    fnord::TaskScheduler* scheduler) :
    file_repo_(new fnord::io::FileRepository(data_dir)),
    compaction_task_(this) {
  std::unordered_map<
      std::string,
      std::vector<std::unique_ptr<TableRef>>> tables;

  file_repo_->listFiles([this, &tables] (const std::string& filename) -> bool {
    fnord::sstable::SSTableRepair repair(filename);

    if (repair.checkAndRepair(true)) {
      auto table_ref = TableRef::openTable(filename);
      tables[table_ref->metricKey()].emplace_back(std::move(table_ref));
    } else {
      /*
      env()->logger()->printf(
          "ERROR",
          "can't repair sstable %s. skipping...",
          filename.c_str());
      */
    }

    return true;
  });

  for (auto& iter : tables) {
    auto metric = new Metric(
        iter.first,
        file_repo_.get(),
        std::move(iter.second));

    metrics_.emplace(iter.first, std::unique_ptr<Metric>(metric));
  }

  scheduler->run(compaction_task_.runnable());
}

Metric* MetricRepository::createMetric(const std::string& key) {
  return new Metric(key, file_repo_.get());
}

}
}
}

