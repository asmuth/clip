/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_METRICREPOSITORY_H_
#define _FNORDMETRIC_METRICDB_METRICREPOSITORY_H_
#include <fnordmetric/io/filerepository.h>
#include <fnordmetric/metricdb/metric.h>
#include <string>
#include <unordered_map>
#include <vector>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

class MetricRepository {
public:

  MetricRepository(std::shared_ptr<io::FileRepository> file_repo);

  Metric* findMetric(const std::string& key);
  Metric* findOrCreateMetric(const std::string& key);

protected:
  std::shared_ptr<io::FileRepository> file_repo_;
  std::unordered_map<std::string, std::unique_ptr<Metric>> metrics_;
};

}
}
#endif
