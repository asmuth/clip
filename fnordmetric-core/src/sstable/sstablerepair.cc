/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sstable/sstablereader.h>
#include <fnordmetric/sstable/sstablerepair.h>
#include <fnordmetric/util/runtimeexception.h>

using fnordmetric::util::RuntimeException;

namespace fnord {
namespace sstable {

SSTableRepair::SSTableRepair(
    const std::string& filename) :
    filename_(filename) {}

bool SSTableRepair::checkAndRepair(bool repair /* = false */) {
  auto file = io::File::openFile(filename_, io::File::O_READ);
  std::unique_ptr<fnord::sstable::SSTableReader> reader_;

  try {
    reader_.reset(new sstable::SSTableReader(std::move(file)));
  } catch (RuntimeException& rte) {
    /* the constructor raises if the checksum is invalid or if the file
     * metadata exceeds the file bounds. there is nothing we can do to recover
     * if that happens */
    return false;
  }

  if (reader_->bodySize() == 0) {
    checkAndRepairUnfinishedTable(repair, reader_.get());
  }

  return true;
}

bool SSTableRepair::checkAndRepairUnfinishedTable(
    bool repair,
    SSTableReader* reader) {
  return true;
}

}
}

