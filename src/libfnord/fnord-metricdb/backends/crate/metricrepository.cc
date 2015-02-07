/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-metricdb/backends/crate/metric.h>
#include <fnord-metricdb/backends/crate/metricrepository.h>

namespace fnord {
namespace metric_service {
namespace crate_backend {

MetricRepository::MetricRepository(
    std::string host) :
    connection_(CrateConnection(URI(host))) {
  std::string find_metrics =
      "select table_name from information_schema.tables where " \
      "schema_name = 'metrics'";

  //connection_.executeQuery(find_metrics,
  //        [this] (const rapidjson::Value& row) -> bool {
  //          if(row.Size() < 1 || !row[0].IsString()){
  //              return false;
  //          }
  //          std::string table = row[0].GetString();
  //          auto metric = new crate_backend::Metric(table, connection_);
  //          metrics_.emplace(table, std::unique_ptr<Metric>(metric));
  //          return true;
  //        });
}

Metric* MetricRepository::createMetric(const std::string& key) {
  std::string create_table = "CREATE TABLE metrics.";
  create_table.append(key);
  create_table.append(" (time timestamp, value double)");
  std::vector<std::string> args;
  connection_.executeQuery(create_table, args);

  return new Metric(key, connection_);
}

}
}
}

