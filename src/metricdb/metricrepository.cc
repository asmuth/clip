/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/metricrepository.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

MetricRepository::MetricRepository(
    std::shared_ptr<io::FileRepository> file_repo) :
    file_repo_(file_repo) {}

Metric* MetricRepository::findMetric(const std::string& key) {
}

Metric* MetricRepository::findOrCreateMetric(const std::string& key) {
}


}
}
