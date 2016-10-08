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
#include <assert.h>
#include <sstream>
#include <metricd/metric.h>
#include <metricd/metric_map.h>
#include <libtsdb/varint.h>

namespace fnordmetric {

MetricConfig::MetricConfig() : is_valid(false) {}

MetricSeries::MetricSeries(
    SeriesIDType series_id,
    LabelSet labels) :
    series_id_(series_id),
    labels_(labels) {}

ReturnCode MetricSeries::insertSample(
    tsdb::TSDB* tsdb,
    Sample sample) {
  if (tsdb->insertUInt64(series_id_, sample.getTime(), sample.getValue())) {
    return ReturnCode::success();
  } else {
    return ReturnCode::error("ERUNTIME", "insert failed");
  }
}

const LabelSet* MetricSeries::getLabels() const {
  return &labels_;
}

bool MetricSeries::hasLabel(const std::string& label) const {
  return labels_.count(label) > 0;
}

std::string MetricSeries::getLabel(const std::string& label) const {
  auto iter = labels_.find(label);
  if (iter == labels_.end()) {
    return "";
  } else {
    return iter->second;
  }
}

bool MetricSeries::compareLabel(
    const std::string& label,
    const std::string& value) const {
  auto iter = labels_.find(label);
  if (iter == labels_.end()) {
    return false;
  } else {
    return iter->second == value;
  }
}

bool MetricSeriesMetadata::encode(std::ostream* os) const {
  if (!tsdb::writeVarUInt(os, metric_id.size())) {
    return false;
  }

  os->write(metric_id.data(), metric_id.size());
  if (os->fail()) {
    return false;
  }

  if (!tsdb::writeVarUInt(os, labels.size())) {
    return false;
  }

  for (const auto& l : labels) {
    if (!tsdb::writeVarUInt(os, l.first.size())) {
      return false;
    }

    os->write(l.first.data(), l.first.size());
    if (os->fail()) {
      return false;
    }

    if (!tsdb::writeVarUInt(os, l.second.size())) {
      return false;
    }

    os->write(l.second.data(), l.second.size());
    if (os->fail()) {
      return false;
    }
  }

  return true;
}

bool MetricSeriesMetadata::decode(std::istream* is) {
  uint64_t metric_id_len;
  if (!tsdb::readVarUInt(is, &metric_id_len)) {
    return false;
  }

  metric_id.resize(metric_id_len);
  is->read(&metric_id[0], metric_id.size());
  if (is->fail()) {
    return false;
  }

  uint64_t labels_count;
  if (!tsdb::readVarUInt(is, &labels_count)) {
    return false;
  }

  for (uint64_t i = 0; i < labels_count; ++i) {
    uint64_t label_key_len;
    if (!tsdb::readVarUInt(is, &label_key_len)) {
      return false;
    }

    std::string label_key(label_key_len, 0);
    is->read(&label_key[0], label_key.size());
    if (is->fail()) {
      return false;
    }

    uint64_t label_value_len;
    if (!tsdb::readVarUInt(is, &label_value_len)) {
      return false;
    }

    std::string label_value(label_value_len, 0);
    is->read(&label_value[0], label_value.size());
    if (is->fail()) {
      return false;
    }

    labels[label_key] = label_value;
  }

  return true;
}

MetricSeriesList::MetricSeriesList() {}

ReturnCode MetricSeriesList::findOrCreateSeries(
    tsdb::TSDB* tsdb,
    SeriesIDProvider* series_id_provider,
    const std::string& metric_id,
    const LabelSet& labels,
    std::shared_ptr<MetricSeries>* series) {
  std::unique_lock<std::mutex> lk(series_mutex_);

  /* try to find an existing series that matches the label set */
  // FIXME this operation should not be O(n)
  auto series_iter = series_.begin();
  for (; series_iter != series_.end(); ++series_iter) {
    if (*series_iter->second->getLabels() == labels) {
      *series = series_iter->second;
      return ReturnCode::success();
    }
  }

  /* if no existing series was found, create a new one */
  auto new_series_id = series_id_provider->allocateSeriesID();
  auto new_series = std::make_shared<MetricSeries>(
      new_series_id,
      labels);

  /* encode series metadata */
  MetricSeriesMetadata metadata;
  metadata.metric_id = metric_id;
  metadata.labels = labels;

  std::ostringstream metadata_buf;
  metadata.encode(&metadata_buf);

  /* create the new  series in the tsdb file */
  auto tsdb_page_type = tsdb::PageType::UINT64; // FIXME
  auto create_rc = tsdb->createSeries(
      new_series_id,
      tsdb_page_type,
      metadata_buf.str());

  if (!create_rc) {
    return ReturnCode::error("ERUNTIME", "can't create series");
  }

  /* add new series to series list */
  series_.emplace(new_series_id, new_series);
  *series = std::move(new_series);
  return ReturnCode::success();
}

void MetricSeriesList::addSeries(
    const SeriesIDType& series_id,
    const LabelSet& labels) {
  assert(series_.count(series_id) == 0);

  series_.emplace(
      series_id,
      std::make_shared<MetricSeries>(
          series_id,
          labels));
}

size_t MetricSeriesList::getSize() const {
  std::unique_lock<std::mutex> lk(series_mutex_);
  return series_.size();
}

Metric::Metric(
    const std::string& key) {}

void Metric::setConfig(MetricConfig config) {
  std::unique_lock<std::mutex> lk(config_mutex_);
  config_ = config;
}

MetricSeriesList* Metric::getSeriesList() {
  return &series_;
}

} // namespace fnordmetric

