/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/random.h>
#include <fnord/base/io/filerepository.h>
#include <fnord/base/io/fileutil.h>

namespace fnord {
namespace io {

FileRepository::FileRepository(
    const std::string& basedir) :
    basedir_(basedir) {}

FileRepository::FileRef FileRepository::createFile() const {
  FileRef fileref;
  fileref.logical_filename = rnd_.alphanumericString(32);
  fileref.absolute_path = FileUtil::joinPaths(
      basedir_,
      fileref.logical_filename);

  return fileref;
}

void FileRepository::listFiles(
    std::function<bool(const std::string&)> callback) const {
  FileUtil::ls(basedir_, [&] (const std::string& filename) -> bool {
    auto absolute_path = FileUtil::joinPaths(basedir_, filename);
    return callback(absolute_path);
  });
}

void FileRepository::deleteAllFiles() {
  listFiles([] (const std::string& filename) -> bool {
    FileUtil::rm(filename);
    return true;
  });
}

}
}

