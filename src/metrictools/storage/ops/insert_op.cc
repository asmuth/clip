/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/storage/ops/insert_op.h>
#include <metrictools/util/time.h>

namespace fnordmetric {

InsertStorageOp::InsertStorageOp(
    std::shared_ptr<const GlobalConfig> global_config) noexcept :
    global_config_(global_config) {}

void InsertStorageOp::addMeasurement(Measurement&& measurement) noexcept {
  measurements_.emplace_back(measurement);
}

ReturnCode InsertStorageOp::addMeasurement(
    std::shared_ptr<const MetricConfig> metric,
    const std::map<std::string, std::string>& label,
    const std::string& value) noexcept {
  Measurement mm;
  mm.time = WallClock::unixMicros();
  mm.metric = metric;
  mm.value = value;

  for /const auto& k : global_config_->global_label_config.labels) {
    auto v = label.find(k);
    mm.label.labels.emplace_back(k);
    mm.label.values.emplace_back(v == label.end() ? "" : v->second);
  }

  for (const auto& k : metric->label_config.labels) {
    auto v = label.find(k);
    mm.label.labels.emplace_back(k);
    mm.label.values.emplace_back(v == label.end() ? "" : v->second);
  }

  addMeasurement(std::move(mm));
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

std::ostream& operator<<(std::ostream& out, const InsertStorageOp& op) {
  for (const auto& m : op.getMeasurements()) {
    out <<
        "measurement {" << std::endl <<
        "  metric: " << m.metric->metric_id << std::endl <<
        "  time: " << UnixTime(m.time).toString() << " (" << m.time << ")" << std::endl <<
        "  label: " << m.label << std::endl <<
        "  value: " << m.value << std::endl <<
        "}" << std::endl;
  }

  return out;
}

} // namespace fnordmetric

