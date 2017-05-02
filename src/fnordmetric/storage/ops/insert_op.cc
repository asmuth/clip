/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/storage/ops/insert_op.h>

namespace fnordmetric {

InsertStorageOp::InsertStorageOp(
    std::shared_ptr<const GlobalConfig> global_config) noexcept :
    global_config_(global_config) {}

void InsertStorageOp::addMeasurement(Measurement&& measurement) noexcept {
  measurements_.emplace_back(measurement);
}

ReturnCode InsertStorageOp::addMeasurement(
    std::shared_ptr<const MetricConfig> metric,
    const std::map<std::string, std::string>& instance,
    const std::string& value) noexcept {
  return ReturnCode::success();
}

const std::vector<InsertStorageOp::Measurement>&
InsertStorageOp::getMeasurements() const noexcept {
  return measurements_;
}

std::shared_ptr<const GlobalConfig>
InsertStorageOp::getGlobalConfig() const noexcept {
  return global_config_;
}

} // namespace fnordmetric

