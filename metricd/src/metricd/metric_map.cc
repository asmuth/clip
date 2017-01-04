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
#include <metricd/metric_map.h>

namespace fnordmetric {

MetricMap::~MetricMap() {
  for (auto& m : metrics_) {
    if (m.second.second) {
      delete m.second.first;
    }
  }
}

Metric* MetricMap::findMetric(const std::string& key) const {
  auto iter = metrics_.find(key);
  if (iter == metrics_.end()) {
    return nullptr;
  } else {
    return iter->second.first;
  }
}

//std::vector<IMetric*> IMetricMap::listMetrics()
//    const {
//  std::vector<IMetric*> metrics;
//
//  {
//    std::lock_guard<std::mutex> lock_holder(metrics_mutex_);
//    for (const auto& iter : metrics_) {
//      metrics.emplace_back(iter.second.get());
//    }
//  }
//
//  return metrics;
//}

MetricMapBuilder::MetricMapBuilder(
    MetricMap* mm) :
    metric_map_(std::make_shared<MetricMap>()) {
  if (mm) {
    for (auto& m : mm->metrics_) {
      m.second.second = false;

      metric_map_->metrics_.emplace(
          m.first,
          std::make_pair(m.second.first, true));
    }

    mm->next_ = metric_map_;
  }
}

Metric* MetricMapBuilder::findMetric(
    const std::string& key) {
  return metric_map_->findMetric(key);
}

void MetricMapBuilder::addMetric(
    const std::string& key,
    std::unique_ptr<Metric> metric) {
  metric_map_->metrics_.emplace(key, std::make_pair(metric.release(), true));
}

std::shared_ptr<MetricMap> MetricMapBuilder::getMetricMap() {
  auto mmap = std::move(metric_map_);
  metric_map_ = std::make_shared<MetricMap>();
  return mmap;
}

std::shared_ptr<MetricMap> VersionedMetricMap::getMetricMap() {
  std::unique_lock<std::mutex> lk(mutex_);
  return metric_map_;
}

void VersionedMetricMap::updateMetricMap(
    std::shared_ptr<MetricMap> metric_map) {
  std::unique_lock<std::mutex> lk(mutex_);
  metric_map_ = std::move(metric_map);
}

MetricListCursor::MetricListCursor(
    std::shared_ptr<MetricMap> metric_map) :
    metric_map_(std::move(metric_map)),
    begin_(metric_map_->metrics_.begin()),
    cur_(begin_),
    end_(metric_map_->metrics_.end()) {}

MetricListCursor::MetricListCursor(
    MetricListCursor&& o) :
    metric_map_(std::move(o.metric_map_)),
    begin_(std::move(o.begin_)),
    cur_(std::move(o.cur_)),
    end_(std::move(o.end_)) {}

const std::string& MetricListCursor::getMetricID() {
  return cur_->first;
}

bool MetricListCursor::isValid() const {
  return cur_ != end_;
}

bool MetricListCursor::next() {
  if (cur_ == end_) {
    return false;
  } else {
    ++cur_;
    return true;
  }
}

SeriesIDProvider::SeriesIDProvider(
    SeriesIDType init_id) :
    series_id_(init_id.id) {}

SeriesIDType SeriesIDProvider::allocateSeriesID() {
  return SeriesIDType(series_id_.fetch_add(1) + 1);
}

} // namespace fnordmetric


