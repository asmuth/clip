/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <fnordmetric/storage/op.h>

namespace fnordmetric {

class InsertStorageOp : public StorageOp {
public:

  struct Measurement {
    uint64_t time;
    std::shared_ptr<MetricConfig> metric;
    std::vector<std::string> tree;
    std::string value;
  };

  void addMeasurement(Measurement&& measurement);
  const std::vector<Measurement> getMeasurements() const;

protected:
  std::vector<Measurement> measurements_;
};

} // namespace fnordmetric

