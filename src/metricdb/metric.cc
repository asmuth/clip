/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/binaryformat.h>
#include <fnordmetric/metricdb/metric.h>
#include <fnordmetric/metricdb/samplefieldindex.h>
#include <fnordmetric/metricdb/samplewriter.h>
#include <fnordmetric/sstable/livesstable.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/util/freeondestroy.h>
#include <fnordmetric/util/wallclock.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

Metric::Metric(
    const std::string& key,
    io::FileRepository* file_repo) :
    key_(key),
    file_repo_(file_repo),
    head_(nullptr) {}


const std::string& Metric::key() const {
  return key_;
}

std::shared_ptr<MetricSnapshot> Metric::getSnapshot() const {
  std::lock_guard<std::mutex> lock_holder(head_mutex_);
  return head_;
}

// Must hold append_mutex_ to call this!
std::shared_ptr<MetricSnapshot> Metric::getOrCreateSnapshot() {
  {
    std::lock_guard<std::mutex> lock_holder(head_mutex_);

    // FIXPAUL test if full...
    if (head_.get() != nullptr) {
      return head_;
    }
  }

  auto new_snapshot = createSnapshot();
  std::lock_guard<std::mutex> lock_holder(head_mutex_);
  head_ = new_snapshot;
  return head_;
}

void Metric::addSample(const Sample<double>& sample) {
  std::lock_guard<std::mutex> lock_holder(append_mutex_);
  auto snapshot = getOrCreateSnapshot();
  auto table = snapshot->liveTable();
  auto field_index = table->getIndex<SampleFieldIndex>();

  SampleWriter writer(field_index);
  writer.writeValue(sample.value);
  for (const auto& label : sample.labels) {
    writer.writeLabel(label.first, label.second);
  }

  uint64_t now = fnord::util::WallClock::unixMillis();
  table->appendRow(&now, sizeof(now), writer.data(), writer.size());
}

std::shared_ptr<MetricSnapshot> Metric::createSnapshot() {
  sstable::IndexProvider indexes;
  indexes.addIndex<SampleFieldIndex>();

  size_t field_definitions_size = 0;
  size_t header_size = sizeof(BinaryFormat::TableHeader) + key_.size();
  void* alloc = malloc(header_size + field_definitions_size);
  fnord::util::FreeOnDestroy autofree(alloc);

  auto header = static_cast<BinaryFormat::TableHeader*>(alloc);
  header->first_timestamp = 123;
  header->metric_key_size = key_.size();
  memcpy(header->metric_key, key_.c_str(), key_.size());

  auto fileref = file_repo_->createFile();
  auto file = io::File::openFile(
      fileref.absolute_path,
      io::File::O_READ | io::File::O_WRITE | io::File::O_CREATE);

  auto live_sstable = sstable::LiveSSTable::create(
      std::move(file),
      std::move(indexes),
      header,
      header_size);

  return std::shared_ptr<MetricSnapshot>(
      new MetricSnapshot(std::move(live_sstable)));
}

void Metric::scanSamples(
    const fnord::util::DateTime& time_begin,
    const fnord::util::DateTime& time_end,
    std::function<bool (SampleReader*)> callback) {
  auto snapshot = getSnapshot();
  // FIXPAUL scan samples from non-live sstables

  if (snapshot.get() == nullptr) {
    return;
  }

  auto cursor = snapshot->liveTable()->getCursor();

  while (cursor->valid()) {
    //void* key;
    //size_t key_size;
    void* data;
    size_t data_size;

    cursor->getData(&data, &data_size);
    SampleReader reader(data, data_size, nullptr);
    callback(&reader);

    if (!cursor->next()) {
      break;
    }
  }
}

}
}
