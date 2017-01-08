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
#include <metricd/metric_cursor.h>
#include <metricd/util/logging.h>
#include <libtsdb/varint.h>

namespace fnordmetric {

MetricConfig::MetricConfig() :
    kind(MetricKind::UNKNOWN),
    granularity(0),
    display_granularity(0),
    is_valid(false) {}

MetricSeries::MetricSeries(
    SeriesIDType series_id,
    SeriesNameType series_name) :
    series_id_(series_id),
    series_name_(series_name) {}

SeriesIDType MetricSeries::getSeriesID() const {
  return series_id_;
}

const SeriesNameType& MetricSeries::getSeriesName() const {
  return series_name_;
}

bool MetricSeriesMetadata::encode(std::ostream* os) const {
  if (!tsdb::writeVarUInt(os, metric_id.size())) {
    return false;
  }

  os->write(metric_id.data(), metric_id.size());
  if (os->fail()) {
    return false;
  }

  if (!tsdb::writeVarUInt(os, (uint64_t) metric_kind)) {
    return false;
  }

  if (!tsdb::writeVarUInt(os, series_name.name.size())) {
    return false;
  }

  os->write(series_name.name.data(), series_name.name.size());
  if (os->fail()) {
    return false;
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

  uint64_t kind;
  if (tsdb::readVarUInt(is, &kind)) {
    metric_kind = (MetricKind) kind;
  } else {
    return false;
  }

  uint64_t series_name_len;
  if (!tsdb::readVarUInt(is, &series_name_len)) {
    return false;
  }

  series_name.name.resize(series_name_len);
  is->read(&series_name.name[0], series_name.name.size());
  if (is->fail()) {
    return false;
  }

  return true;
}

MetricSeriesList::MetricSeriesList() {}

bool MetricSeriesList::findSeries(
    const SeriesIDType& series_id,
    std::shared_ptr<MetricSeries>* series) {
  std::unique_lock<std::mutex> lk(series_mutex_);

  auto iter = series_by_id_.find(series_id.id);
  if (iter == series_by_id_.end()) {
    return false;
  } else {
    *series = iter->second;
    return true;
  }
}

bool MetricSeriesList::findSeries(
    const SeriesNameType& series_name,
    std::shared_ptr<MetricSeries>* series) {
  std::unique_lock<std::mutex> lk(series_mutex_);

  auto iter = series_.find(series_name.name);
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
    const SeriesNameType& series_name,
    std::shared_ptr<MetricSeries>* series) {
  std::unique_lock<std::mutex> lk(series_mutex_);

  /* try to find an existing series that matches the label set */
  // FIXME this operation should not be O(n)
  auto series_iter = series_.find(series_name.name);
  if (series_iter != series_.end()) {
    *series = series_iter->second;
    return ReturnCode::success();
  }

  /* if no existing series was found, create a new one */
  auto new_series_id = series_id_provider->allocateSeriesID();

  auto new_series = std::make_shared<MetricSeries>(
      new_series_id,
      series_name);

  /* encode series metadata */
  MetricSeriesMetadata metadata;
  metadata.metric_id = metric_id;
  metadata.metric_kind = config.kind;
  metadata.series_name = series_name;

  std::ostringstream metadata_buf;
  metadata.encode(&metadata_buf);

  /* create the new  series in the tsdb file */
  auto create_rc = tsdb->createSeries(
      new_series_id.id,
      tval_len(getMetricDataType(config.kind)),
      metadata_buf.str());

  if (!create_rc) {
    return ReturnCode::error("ERUNTIME", "can't create series");
  }

  /* add new series to series list */
  series_.emplace(series_name.name, new_series);
  series_by_id_.emplace(new_series_id.id, new_series);
  *series = std::move(new_series);
  return ReturnCode::success();
}

void MetricSeriesList::addSeries(
    const SeriesIDType& series_id,
    const SeriesNameType& series_name) {
  std::unique_lock<std::mutex> lk(series_mutex_);
  assert(series_.count(series_name.name) == 0);

  auto series = std::make_shared<MetricSeries>(
      series_id,
      series_name);

  series_.emplace(series_name.name, series);
  series_by_id_.emplace(series_id.id, series);
}

void MetricSeriesList::listSeries(std::vector<SeriesIDType>* series_ids) {
  std::unique_lock<std::mutex> lk(series_mutex_);
  series_ids->reserve(series_by_id_.size());
  for (const auto& s : series_by_id_) {
    series_ids->emplace_back(SeriesIDType(s.first));
  }
}

MetricSeriesListCursor MetricSeriesList::listSeries() {
  std::unique_lock<std::mutex> lk(series_mutex_);
  std::vector<SeriesIDType> snapshot;
  snapshot.reserve(series_by_id_.size());
  for (const auto& s : series_by_id_) {
    snapshot.emplace_back(SeriesIDType(s.first));
  }
  lk.unlock();

  return MetricSeriesListCursor(
      this,
      std::move(snapshot));
}

size_t MetricSeriesList::getSize() const {
  std::unique_lock<std::mutex> lk(series_mutex_);
  return series_.size();
}

MetricSeriesListCursor::MetricSeriesListCursor() :
    valid_(false),
    series_list_(nullptr) {}

MetricSeriesListCursor::MetricSeriesListCursor(
    MetricSeriesList* series_list,
    ListType&& snapshot) :
    valid_(true),
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

const SeriesNameType& MetricSeriesListCursor::getSeriesName() const {
  assert(valid_ && series_);
  return series_->getSeriesName();
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

void MetricSeriesListCursor::setMetricMap(
    std::shared_ptr<MetricMap> metric_map) {
  metric_map_ = std::move(metric_map);
}

Metric::Metric(
    const std::string& key) :
    key_(key),
    unit_config_(nullptr) {}

ReturnCode Metric::setConfig(MetricConfig config) {
  //if (config.aggregation != MetricAggregationType::NONE &&
  //    config.granularity == 0) {
  //  logWarning(
  //      "metric<$0>: setting 'aggregation' without 'granularity' will have "
  //      "no effect",
  //      key_);
  //}

  if (config.kind == MetricKind::UNKNOWN) {
    return ReturnCode::errorf(
        "EARG",
        "metric<$0>: missing 'kind'",
        key_);
  }

  config_ = config;
  input_aggr_ = mkInputAggregator(&config_);
  return ReturnCode::success();
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

void Metric::setUnitConfig(const UnitConfig* config) {
  unit_config_ = config;
}

const UnitConfig* Metric::getUnitConfig() const {
  return unit_config_;
}

MetricInfo::MetricInfo() : metric_(nullptr) {}

MetricInfo::MetricInfo(
    Metric* metric,
    std::shared_ptr<MetricMap> metric_map) :
    metric_(metric),
    metric_map_(std::move(metric_map)) {}


MetricInfo::MetricInfo(
    MetricInfo&& o) :
    metric_(o.metric_),
    metric_map_(std::move(o.metric_map_)) {
  o.metric_ = nullptr;
}

MetricInfo& MetricInfo::operator=(MetricInfo&& o) {
  metric_map_ = std::move(o.metric_map_);
  metric_ = o.metric_;
  o.metric_ = nullptr;
}

const UnitConfig* MetricInfo::getUnitConfig() const {
  if (!metric_) {
    return nullptr;
  }

  return metric_->getUnitConfig();
}

MetricConfig MetricInfo::getMetricConfig() const {
  //if (!metric_) {
  //  return nullptr;
  //}

  return metric_->getConfig();
}

tval_type getMetricDataType(MetricKind t) {
  switch (t) {
    case SAMPLE_UINT64: return tval_type::UINT64;
    case SAMPLE_INT64: return tval_type::INT64;
    case SAMPLE_FLOAT64: return tval_type::FLOAT64;
    case COUNTER_UINT64: return tval_type::UINT64;
    case COUNTER_INT64: return tval_type::INT64;
    case COUNTER_FLOAT64: return tval_type::FLOAT64;
    case MONOTONIC_UINT64: return tval_type::UINT64;
    case MONOTONIC_INT64: return tval_type::INT64;
    case MONOTONIC_FLOAT64: return tval_type::FLOAT64;
    case MIN_UINT64: return tval_type::UINT64;
    case MIN_INT64: return tval_type::INT64;
    case MIN_FLOAT64: return tval_type::FLOAT64;
    case MAX_UINT64: return tval_type::UINT64;
    case MAX_INT64: return tval_type::INT64;
    case MAX_FLOAT64: return tval_type::FLOAT64;
    case AVERAGE_UINT64: return tval_type::UINT64;
    case AVERAGE_INT64: return tval_type::INT64;
    case AVERAGE_FLOAT64: return tval_type::FLOAT64;

    case UNKNOWN:
    default:
      return tval_type::UINT64;
  }
}

} // namespace fnordmetric

