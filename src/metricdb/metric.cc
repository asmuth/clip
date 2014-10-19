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
    live_sstable_(nullptr) {}

void Metric::addSample(const Sample<double>& sample) {
  SampleWriter writer;
  writer.writeValue(sample.value);

  auto live_table = getLiveTable();

  uint64_t now = fnord::util::WallClock::unixMillis();
  live_table->appendRow(&now, sizeof(now), writer.data(), writer.size());
}

const std::string& Metric::key() const {
  return key_;
}

sstable::LiveSSTable* Metric::getLiveTable() {
  if (live_sstable_.get() == nullptr) {
    mkLiveTable();
  }

  return live_sstable_.get();
}

void Metric::mkLiveTable() {
  sstable::IndexProvider indexes;

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

  live_sstable_ = sstable::LiveSSTable::create(
      std::move(file),
      std::move(indexes),
      header,
      header_size);
}

void Metric::scanSamples(
    const fnord::util::DateTime& time_begin,
    const fnord::util::DateTime& time_end,
    std::function<bool (Sample<double> const*)> callback) {
  // FIXPAUL scan samples from non-live sstables

  // FIXPAUL locking
  if (live_sstable_.get() != nullptr) {
    auto cursor = live_sstable_->getCursor();

    while (cursor->valid()) {
      callback(nullptr);

      if (!cursor->next()) {
        break;
      }
    }
  }
}

}
}
