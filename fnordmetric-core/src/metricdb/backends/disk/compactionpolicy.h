/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_COMPACTIONPOLICY_H_
#define _FNORDMETRIC_METRICDB_COMPACTIONPOLICY_H_
#include <fnordmetric/metricdb/backends/disk/tableref.h>

namespace fnordmetric {
namespace metricdb {
namespace disk_backend {

class CompactionPolicy {
public:
  CompactionPolicy() {}
  virtual ~CompactionPolicy() {}

  virtual void compact(std::vector<std::shared_ptr<TableRef>>* tables) = 0;

};

}
}
}
#endif
