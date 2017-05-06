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
#include <metrictools/util/time.h>

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

LabelSet& Measurement::getLabels() {
  return labels_;
}

std::string getMeasurementCodingName(MeasurementCoding t) {
  switch (t) {
    case MeasurementCoding::STATSD: return "statsd";
    case MeasurementCoding::JSON: return "json";
    case MeasurementCoding::BORGMON_PROMETHEUS: return "borgmon";
  }

  return "???";
}

bool parseMeasurementCoding(
    const std::string& s,
    MeasurementCoding* t) {
  if (s == "statsd") { *t = MeasurementCoding::STATSD; return true; }
  if (s == "json") { *t = MeasurementCoding::JSON; return true; }
  if (s == "borgmon") { *t = MeasurementCoding::BORGMON_PROMETHEUS; return true; }
  if (s == "prometheus") { *t = MeasurementCoding::BORGMON_PROMETHEUS; return true; }
  return false;
}

static ReturnCode parseMeasurements_BORGMON(
    const std::string& input,
    std::vector<Measurement>* samples) {
  const char* cur = input.c_str();
  const char* end = cur + input.size();
  while (cur != end) {
    /* read metric id */
    std::string metric_id;
    while (cur != end) {
      if (*cur == '{' || *cur == ' ') {
        break;
      } else {
        metric_id.push_back(*cur++);
      }
    }

    if (cur == end) {
      break;
    }

    /* read labels */
    LabelSet labels;
    if (*cur == '{') {
      while (cur != end && (*cur == '{' || *cur == ',')) {
        do { ++cur; } while (*cur == ' ');

        /* read label name */
        std::string label_name;
        while (cur != end && StringUtil::isShellSafe(*cur)) { 
          label_name.push_back(*cur++);
        }

        /* skip whitespace */
        for (; cur != end && *cur == ' '; ++cur);

        /* expect equal sign */
        if (cur == end || *cur != '=') {
          return ReturnCode::error("ERUNTIME", "invalid encoding");
        }

        /* skip whitespace */
        do { ++cur; } while (*cur == ' ');

        /* leading double quote */
        if (cur == end || *cur != '"') {
          return ReturnCode::error(
              "ERUNTIME",
              "invalid encoding (missing double quotes around label value?)");
        } else {
          ++cur;
        }

        /* read label value */
        std::string label_value;
        while (cur != end && *cur != '"') {
          label_value.push_back(*cur++);
        }

        /* closing double quote */
        if (cur == end || *cur != '"') {
          return ReturnCode::error("ERUNTIME", "invalid encoding");
        }

        /* skip whitespace */
        do { ++cur; } while (*cur == ' ');

        labels.emplace(label_name, label_value);
      }

      if (cur == end || *cur++ != '}') {
        return ReturnCode::error("ERUNTIME", "invalid encoding");
      }
    }

    /* skip whitespace */
    for (; cur != end && *cur == ' '; ++cur);

    /* read value */
    std::string value;
    while (cur != end && !(*cur == '\n' || *cur == '\r' || *cur == ' ' || !*cur)) { 
      value.push_back(*cur++);
    }

    /* skip whitespace */
    for (; cur != end && (*cur == ' ' || *cur == '\n' || *cur == '\r'); ++cur);

    /* add measurement */
    if (!metric_id.empty() && !value.empty()) {
      Measurement measurement(
          metric_id,
          labels,
          WallClock::unixMicros(),
          value);

      samples->emplace_back(std::move(measurement));
    }
  }

  return ReturnCode::success();
}

ReturnCode parseMeasurements(
    MeasurementCoding format,
    const std::string& input,
    std::vector<Measurement>* samples) {
  switch (format) {
    case MeasurementCoding::STATSD:
      return parseStatsdMeasurements(input.data(), input.size(), samples);
    case MeasurementCoding::BORGMON_PROMETHEUS:
      return parseMeasurements_BORGMON(input, samples);
    default:
      return ReturnCode::error("ERUNTIME", "invalid format");
  }
}

static void applyLabelOverrides(
    const std::vector<MetricLabelOverride>& overrides,
    Measurement* measurement,
    bool defaults) {
  for (const auto& o : overrides) {
    auto& labels = measurement->getLabels();
    if (defaults == o.is_default && (!o.is_default || labels.count(o.label) == 0)) {
      labels[o.label] = o.value;
    }
  }
}

ReturnCode rewriteMeasurements(
    const ConfigList* config,
    const std::vector<MetricLabelOverride>* extra_label_overrides,
    std::vector<Measurement>* measurements) {
  auto global_cfg = config->getGlobalConfig();
  for (auto& m : *measurements) {
    auto metric_cfg = config->getMetricConfig(m.getMetricID());
    if (!metric_cfg) {
      continue;
    }

    if (extra_label_overrides) {
      applyLabelOverrides(*extra_label_overrides, &m, true);
    }
    applyLabelOverrides(metric_cfg->label_overrides, &m, true);
    applyLabelOverrides(global_cfg->global_label_overrides, &m, true);

    applyLabelOverrides(global_cfg->global_label_overrides, &m, false);
    applyLabelOverrides(metric_cfg->label_overrides, &m, false);
    if (extra_label_overrides) {
      applyLabelOverrides(*extra_label_overrides, &m, false);
    }
  }

  return ReturnCode::success();
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

std::ostream& operator<<(std::ostream& out, const Measurement& m) {
  out <<
      "measurement {" << std::endl <<
      "  metric: " << m.getMetricID() << std::endl <<
      "  time: " << UnixTime(m.getTime()).toString() << " (" << m.getTime() << ")" << std::endl <<
      "  labels: " << m.getLabels() << std::endl <<
      "  value: " << m.getValue() << std::endl <<
      "}" << std::endl;

  return out;
}

std::ostream& operator<<(std::ostream& out, const LabelSet& labels) {
  out << "[";
  size_t i = 0;
  for (const auto& l : labels) {
    if (i++ > 0) {
      out << ", ";
    }

    out << "<" << l.first << ": '" << l.second << "'>";
  }

  out << "]";
  return out;
}

} // namespace fnordmetric

