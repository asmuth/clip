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
#include <metricd/metric.h>
#include <metricd/metric_map.h>

namespace fnordmetric {

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

MetricSeriesList::MetricSeriesList() {}

ReturnCode MetricSeriesList::findOrCreateSeries(
    tsdb::TSDB* tsdb,
    SeriesIDProvider* series_id_provider,
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

  auto tsdb_page_type = tsdb::PageType::UINT64; // FIXME
  if (!tsdb->createSeries(new_series_id, tsdb_page_type)) {
    return ReturnCode::error("ERUNTIME", "can't create series");
  }

  series_.emplace(new_series_id, new_series);
  *series = std::move(new_series);
  return ReturnCode::success();
}

size_t MetricSeriesList::getSize() const {
  std::unique_lock<std::mutex> lk(series_mutex_);
  return series_.size();
}

Metric::Metric(
    const std::string& key) {}

MetricSeriesList* Metric::getSeriesList() {
  return &series_;
}

} // namespace fnordmetric

