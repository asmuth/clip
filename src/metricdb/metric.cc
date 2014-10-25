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
#include <fnordmetric/metricdb/tableref.h>
#include <fnordmetric/sstable/livesstable.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/util/freeondestroy.h>
#include <fnordmetric/util/wallclock.h>
#include <string.h>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

Metric::Metric(
    const std::string& key,
    io::FileRepository* file_repo) :
    key_(key),
    file_repo_(file_repo),
    head_(nullptr),
    max_generation_(0) {}

Metric::Metric(
    const std::string& key,
    io::FileRepository* file_repo,
    std::vector<std::unique_ptr<TableRef>>&& tables) :
    key_(key),
    file_repo_(file_repo) {
  std::shared_ptr<MetricSnapshot> snapshot(new MetricSnapshot());

  // FIXPAUL sort tables
  for (auto& table : tables) {
    snapshot->appendTable(std::move(table));
  }

  head_ = snapshot;
}

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
  //auto field_index = table->getIndex<SampleFieldIndex>();

  SampleWriter writer(nullptr);
  writer.writeValue(sample.value);
  for (const auto& label : sample.labels) {
    writer.writeLabel(label.first, label.second);
  }

  std::lock_guard<std::mutex> lock_holder(append_mutex_);
  auto snapshot = getOrCreateSnapshot();
  auto& table = snapshot->tables().back();

  uint64_t now = fnord::util::WallClock::unixMillis();
  table->addSample(&writer, now);
}

// FIXPAUL misnomer...it creates a new snapshot + appends a new, clean table
std::shared_ptr<MetricSnapshot> Metric::createSnapshot() {
  std::shared_ptr<MetricSnapshot> snapshot;
  std::vector<uint64_t> parents;

  if (head_.get() == nullptr) {
    snapshot.reset(new MetricSnapshot());
  } else {
    snapshot = head_->clone();
  }

  // open new file
  auto fileref = file_repo_->createFile();
  auto file = io::File::openFile(
      fileref.absolute_path,
      io::File::O_READ | io::File::O_WRITE | io::File::O_CREATE);

  snapshot->appendTable(TableRef::createTable(
      std::move(file),
      key_,
      max_generation_++,
      parents));

  return snapshot;
}

void Metric::scanSamples(
    const fnord::util::DateTime& time_begin,
    const fnord::util::DateTime& time_end,
    std::function<bool (MetricCursor*)> callback) {
  auto snapshot = getSnapshot();
  if (snapshot.get() == nullptr) {
    return;
  }

  MetricCursor cursor(snapshot);
  while (cursor.valid()) {
    callback(&cursor);

    if (!cursor.next()) {
      break;
    }
  }
}

}
}
