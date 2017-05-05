/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/measure.h>
#include <metrictools/statsd.h>
#include <metrictools/storage/backend.h>
#include <metrictools/storage/ops/insert_op.h>
#include <metrictools/util/logging.h>

namespace fnordmetric {

Measurement::Measurement(
    const std::string& metric_id,
    const LabelSet& labels,
    TimestampType time,
    const std::string& value) :
    metric_id_(metric_id),
    labels_(labels),
    time_(time),
    value_(value) {}

const std::string& Measurement::getMetricID() const {
  return metric_id_;
}

const std::string& Measurement::getValue() const {
  return value_;
}

TimestampType Measurement::getTime() const {
  return time_;
}

const LabelSet& Measurement::getLabels() const {
  return labels_;
}

std::string getMeasurementCodingName(MeasurementCoding t) {
  switch (t) {
    case MeasurementCoding::STATSD: return "statsd";
    case MeasurementCoding::JSON: return "json";
  }

  return "???";
}

bool parseMeasurementCoding(
    const std::string& s,
    MeasurementCoding* t) {
  if (s == "statsd") { *t = MeasurementCoding::STATSD; return true; }
  if (s == "json") { *t = MeasurementCoding::JSON; return true; }
  return false;
}

ReturnCode parseMeasurements(
    MeasurementCoding format,
    const std::string& input,
    std::vector<Measurement>* samples) {
  switch (format) {
    case MeasurementCoding::STATSD:
      return parseStatsdMeasurements(input.data(), input.size(), samples);
    default:
      return ReturnCode::error("ERUNTIME", "invalid format");
  }
}

ReturnCode storeMeasurements(
    const ConfigList* config,
    Backend* storage_backend,
    const std::vector<Measurement>& samples) {
  InsertStorageOp op(config->getGlobalConfig());
  for (const auto& s : samples) {
    auto metric = config->getMetricConfig(s.getMetricID());
    if (!metric) {
      logWarning(
          "batch insert failed: metric not found: '$0'",
          s.getMetricID());
      continue;
    }

    auto rc = op.addMeasurement(metric, s.getLabels(), s.getValue());
    if (!rc.isSuccess()) {
      logWarning(
          "batch insert failed: $0; metric_id=$1 value=$2",
          rc.getMessage(),
          s.getMetricID(),
          s.getValue());
    }
  }

  return storage_backend->performOperation(&op);
}

} // namespace fnordmetric

