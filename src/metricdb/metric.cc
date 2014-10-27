/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnordmetric/metricdb/binaryformat.h>
#include <fnordmetric/metricdb/metric.h>
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
    max_generation_(0),
    live_table_max_size_(kLiveTableMaxSize),
    live_table_idle_time_micros_(kLiveTableIdleTimeMicros),
    last_insert_(0) {
  if (env()->verbose()) {
    env()->logger()->printf(
        "DEBUG",
        "Create new metric: '%s'",
        key.c_str());
  }
}

Metric::Metric(
    const std::string& key,
    io::FileRepository* file_repo,
    std::vector<std::unique_ptr<TableRef>>&& tables) :
    key_(key),
    file_repo_(file_repo),
    live_table_max_size_(kLiveTableMaxSize),
    live_table_idle_time_micros_(kLiveTableIdleTimeMicros),
    last_insert_(0) {
  TableRef* head_table = nullptr;
  std::vector<uint64_t> generations;

  for (auto& table : tables) {
    if (head_table == nullptr ||
        table->generation() > head_table->generation()) {
      head_table = table.get();
    }
  }

  generations = head_table->parents();
  generations.emplace_back(head_table->generation());

  if (env()->verbose()) {
    env()->logger()->printf(
        "DEBUG",
        "Reopening metric: '%s' with %i table(s), generation: %" PRIu64,
        key.c_str(),
        (int) generations.size(),
        head_table->generation());
  }

  std::shared_ptr<MetricSnapshot> snapshot(new MetricSnapshot());
  for (const auto gen : generations) {
    for (auto& table : tables) {
      if (table.get() == nullptr) {
        continue;
      }

      if (table->generation() == gen) {
        table->importTokenIndex(&token_index_);
        snapshot->appendTable(std::move(table));
        table.reset(nullptr);
      }
    }
  }

  head_ = snapshot;
  max_generation_ = head_table->generation();

  for (auto& table : tables) {
    if (table.get() != nullptr) {
      env()->logger()->printf("INFO", "Orphaned table...");
    }
  }
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

    if (head_.get() != nullptr &&
        head_->isWritable() &&
        head_->tables().back()->isWritable() &&
        head_->tables().back()->bodySize() < live_table_max_size_) {
      return head_;
    }
  }

  std::lock_guard<std::mutex> lock_holder(head_mutex_);
  auto new_snapshot = createSnapshot(true);
  head_ = new_snapshot;
  return head_;
}

void Metric::addSample(const Sample<double>& sample) {
  SampleWriter writer(&token_index_);
  writer.writeValue(sample.value);
  for (const auto& label : sample.labels) {
    writer.writeLabel(label.first, label.second);
    label_index_.addLabel(label.first);
  }

  std::lock_guard<std::mutex> lock_holder(append_mutex_);
  auto snapshot = getOrCreateSnapshot();
  auto& table = snapshot->tables().back();

  uint64_t now = fnord::util::WallClock::unixMicros();
  table->addSample(&writer, now);
  last_insert_ = now;
}

// FIXPAUL misnomer...it creates a new snapshot + appends a new, clean table
std::shared_ptr<MetricSnapshot> Metric::createSnapshot(bool writable) {
  std::shared_ptr<MetricSnapshot> snapshot;
  std::vector<uint64_t> parents;

  if (head_.get() == nullptr) {
    snapshot.reset(new MetricSnapshot());
  } else {
    snapshot = head_->clone();
    for (const auto& tbl : snapshot->tables()) {
      parents.emplace_back(tbl->generation());
    }
  }

  snapshot->setWritable(writable);

  if (writable) {
    // open new file
    auto fileref = file_repo_->createFile();
    auto file = io::File::openFile(
        fileref.absolute_path,
        io::File::O_READ | io::File::O_WRITE | io::File::O_CREATE);

    snapshot->appendTable(TableRef::createTable(
        fileref.absolute_path,
        key_,
        std::move(file),
        ++max_generation_,
        parents));
  }

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

  MetricCursor cursor(snapshot, &token_index_);
  while (cursor.valid()) {
    callback(&cursor);

    if (!cursor.next()) {
      break;
    }
  }
}

void Metric::compact(CompactionPolicy* compaction /* = nullptr */) {
  if (!compaction_mutex_.try_lock()) {
    return;
  }

  std::lock_guard<std::mutex>(compaction_mutex_, std::adopt_lock);

  if (env()->verbose()) {
    env()->logger()->printf(
        "DEBUG",
        "Running compaction for metric: '%s'",
        key_.c_str());
  }

  // create a new snapshot and make all tables immutable
  std::shared_ptr<MetricSnapshot> snapshot;
  {
    std::lock_guard<std::mutex> append_lock_holder(append_mutex_);
    std::lock_guard<std::mutex> head_lock_holder(head_mutex_);
    snapshot = createSnapshot(false);
  }

  auto old_tables = snapshot->tables();
  auto idle_time_micros = WallClock::unixMicros() - last_insert_;

  if (idle_time_micros < live_table_idle_time_micros_) {
    old_tables.pop_back();
  }

  std::vector<std::shared_ptr<TableRef>> new_tables;

  // finalize unfinished sstables
  for (auto& table : old_tables) {
    if (table->isWritable()) {
      if (table->bodySize() == 0) {
        if (env()->verbose()) {
          env()->logger()->printf(
              "DEBUG",
              "SSTable '%s' (%s) is empty, deleting...",
              table->filename().c_str(),
              table->metricKey().c_str());
        }

        continue;
      } else {
        if (env()->verbose()) {
          env()->logger()->printf(
              "DEBUG",
              "Finalizing sstable '%s' (%s)",
              table->filename().c_str(),
              table->metricKey().c_str());
        }

        table->finalize(&token_index_);
        new_tables.emplace_back(new ReadonlyTableRef(*table));
      }
    } else {
      new_tables.emplace_back(table);
    }
  }

  // run the compaction
  if (compaction != nullptr) {
    compaction->compact(&new_tables);
  }

  // create a new snapshot and commit modifications
  {
    std::lock_guard<std::mutex> append_lock_holder(append_mutex_);
    std::lock_guard<std::mutex> head_lock_holder(head_mutex_);
    auto new_snapshot = new MetricSnapshot();
    new_snapshot->setWritable(head_->isWritable());

    for (const auto& table : new_tables) {
      new_snapshot->appendTable(table);
    }

    for (int i = old_tables.size(); i < head_->tables().size(); ++i) {
      new_snapshot->appendTable(head_->tables()[i]);
    }

    head_.reset(new_snapshot);
  }
}

void Metric::setLiveTableMaxSize(size_t max_size) {
  live_table_max_size_ = max_size;
}

void Metric::setLiveTableIdleTimeMicros(uint64_t idle_time_micros) {
  live_table_idle_time_micros_ = idle_time_micros;
}

size_t Metric::numTables() const {
  auto snapshot = getSnapshot();
  return snapshot->tables().size();
}

size_t Metric::totalBytes() const {
  auto snapshot = getSnapshot();

  size_t bytes = 0;
  for (const auto& tbl : snapshot->tables()) {
    bytes += tbl->bodySize();
  }

  return bytes;
}

uint64_t Metric::lastInsertTime() const {
  return last_insert_;
}

std::set<std::string> Metric::labels() const {
  return label_index_.labels();
}

}
}
