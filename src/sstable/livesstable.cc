/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sstable/livesstable.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnord {
namespace sstable {

std::unique_ptr<LiveSSTable> LiveSSTable::create(
    io::File file,
    IndexProvider index_provider,
    void const* header,
    size_t header_size) {

  return std::unique_ptr<LiveSSTable>(
      new LiveSSTable(std::move(file), index_provider.popIndexes()));
}

LiveSSTable::LiveSSTable(
    io::File&& file,
    std::vector<Index::IndexRef>&& indexes) :
    file_(std::move(file)),
    indexes_(std::move(indexes)) {}

LiveSSTable::~LiveSSTable() {
}

/*
std::unique_ptr<LiveSSTable> LiveSSTable::reopen(
    io::File file,
    const IndexProvider& index_provider) {}

void LiveSSTable::appendRow(
    void const* key,
    size_t key_size,
    void const* data,
    size_t data_size) {}

void LiveSSTable::appendRow(
    const std::string& key,
    const std::string& value) {}

void LiveSSTable::finalize() {}
*/

}
}
