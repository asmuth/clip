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
#include <metricd/util/logging.h>
#include <libtsdb/varint.h>

namespace fnordmetric {

MetricConfig::MetricConfig() :
   data_type(MetricDataType::UINT64),
   aggregation(MetricAggregationType::NONE),
   granularity(0),
   display_granularity(0),
   is_valid(false) {}

SeriesIDType MetricSeries::getSeriesID() const {
  return series_id_;
}

MetricSeries::MetricSeries(
    SeriesIDType series_id,
    LabelSet labels) :
    series_id_(series_id),
    labels_(labels) {}

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

bool MetricSeriesList::findSeries(
    SeriesIDType series_id,
    std::shared_ptr<MetricSeries>* series) {
  std::unique_lock<std::mutex> lk(series_mutex_);

  auto iter = series_.find(series_id);
  if (iter == series_.end()) {
    return false;
  } else {
    *series = iter->second;
    return true;
  }
}

ReturnCode MetricSeriesList::findOrCreateSeries(
    tsdb::TSDB* tsdb,
    SeriesIDProvider* series_id_provider,
    const std::string& metric_id,
    const MetricConfig& config,
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
  auto create_rc = tsdb->createSeries(
      new_series_id,
      getMetricDataTypeSize(config.data_type),
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
  std::unique_lock<std::mutex> lk(series_mutex_);
  assert(series_.count(series_id) == 0);

  series_.emplace(
      series_id,
      std::make_shared<MetricSeries>(
          series_id,
          labels));
}

void MetricSeriesList::listSeries(std::vector<SeriesIDType>* series_ids) {
  std::unique_lock<std::mutex> lk(series_mutex_);
  series_ids->reserve(series_.size());
  for (const auto& s : series_) {
    series_ids->emplace_back(s.first);
  }
}

size_t MetricSeriesList::getSize() const {
  std::unique_lock<std::mutex> lk(series_mutex_);
  return series_.size();
}

MetricSeriesCursor::MetricSeriesCursor() {}

MetricSeriesCursor::MetricSeriesCursor(
    const MetricConfig* config,
    tsdb::Cursor cursor) :
    cursor_(std::move(cursor)),
    aggr_(mkOutputAggregator(&cursor_, config)) {}

MetricSeriesCursor::MetricSeriesCursor(
    MetricSeriesCursor&& o) :
    cursor_(std::move(o.cursor_)),
    aggr_(std::move(o.aggr_)) {}

MetricSeriesCursor& MetricSeriesCursor::operator=(MetricSeriesCursor&& o) {
  cursor_ = std::move(o.cursor_);
  aggr_ = std::move(o.aggr_);
  return *this;
}

bool MetricSeriesCursor::next(uint64_t* timestamp, uint64_t* value) {
  if (aggr_) {
    return aggr_->next(timestamp, value, sizeof(uint64_t));
  } else {
    return cursor_.next(timestamp, value, sizeof(uint64_t));
  }
}

std::unique_ptr<InputAggregator> mkInputAggregator(
    const MetricConfig* config) {
  if (config->granularity == 0) {
    return {};
  }

  switch (config->aggregation) {
    case MetricAggregationType::SUM:
      return std::unique_ptr<InputAggregator>(
          new SumInputAggregator(config->granularity));
    case MetricAggregationType::NONE: return {};
    default: return {};
  }
}

std::unique_ptr<OutputAggregator> mkOutputAggregator(
    tsdb::Cursor* cursor,
    const MetricConfig* config) {
  uint64_t granularity = config->display_granularity;
  if (granularity == 0) {
    granularity = config->granularity;
  }

  if (granularity == 0) {
    return {};
  }

  switch (config->aggregation) {
    case MetricAggregationType::SUM:
      return std::unique_ptr<OutputAggregator>(
          new SumOutputAggregator(cursor, granularity));
    case MetricAggregationType::NONE: return {};
    default: return {};
  }
}

MetricSeriesListCursor::MetricSeriesListCursor() :
    valid_(false),
    series_list_(nullptr) {}

MetricSeriesListCursor::MetricSeriesListCursor(
    std::shared_ptr<MetricMap> metric_map,
    MetricSeriesList* series_list,
    ListType&& snapshot) :
    valid_(true),
    metric_map_(metric_map),
    series_list_(series_list),
    snapshot_(std::move(snapshot)),
    cursor_(snapshot_.begin()) {
  fetchNext();
}

MetricSeriesListCursor::MetricSeriesListCursor(
    MetricSeriesListCursor&& o) :
    valid_(o.valid_),
    metric_map_(std::move(o.metric_map_)),
    series_list_(o.series_list_) {
  o.valid_ = false;
  o.series_list_ = nullptr;
  if (valid_) {
    auto cursor_pos = o.cursor_ - o.snapshot_.begin();
    snapshot_ = std::move(o.snapshot_);
    cursor_ = snapshot_.begin() + cursor_pos;
    fetchNext();
  }
}

MetricSeriesListCursor& MetricSeriesListCursor::operator=(
    MetricSeriesListCursor&& o) {
  valid_ = o.valid_;
  o.valid_ = false;
  metric_map_ = std::move(o.metric_map_);
  series_list_ = o.series_list_;
  o.series_list_ = nullptr;
  if (valid_) {
    auto cursor_pos = o.cursor_ - o.snapshot_.begin();
    snapshot_ = std::move(o.snapshot_);
    cursor_ = snapshot_.begin() + cursor_pos;
    fetchNext();
  }

  return *this;
}

SeriesIDType MetricSeriesListCursor::getSeriesID() const {
  assert(valid_);
  return *cursor_;
}

const LabelSet* MetricSeriesListCursor::getLabels() const {
  assert(valid_);
  assert(series_.get() != nullptr);
  return series_->getLabels();
}

bool MetricSeriesListCursor::isValid() const {
  return valid_ && cursor_ != snapshot_.end();
}

bool MetricSeriesListCursor::next() {
  if (!valid_ || cursor_ == snapshot_.end()) {
    return false;
  } else {
    ++cursor_;
    return fetchNext();
  }
}

bool MetricSeriesListCursor::fetchNext() {
  while (cursor_ != snapshot_.end()) {
    if (series_list_->findSeries(*cursor_, &series_)) {
      return true;
    }
  }

  return false;
}

Metric::Metric(
    const std::string& key) :
    key_(key) {}

void Metric::setConfig(MetricConfig config) {
  if (config.aggregation != MetricAggregationType::NONE &&
      config.granularity == 0) {
    logWarning(
        "metric<$0>: setting 'aggregation' without 'granularity' will have "
        "no effect",
        key_);
  }

  config_ = config;
  input_aggr_ = mkInputAggregator(&config_);
}

const MetricConfig& Metric::getConfig() const {
  return config_;
}

MetricSeriesList* Metric::getSeriesList() {
  return &series_;
}

InputAggregator* Metric::getInputAggregator() {
  return input_aggr_.get();
}

} // namespace fnordmetric

