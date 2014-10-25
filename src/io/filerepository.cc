/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/io/filerepository.h>
#include <fnordmetric/io/fileutil.h>
#include <fnordmetric/util/random.h>

namespace fnord {
namespace io {

FileRepository::FileRepository(
    const std::string& basedir) :
    basedir_(basedir) {}

FileRepository::FileRef FileRepository::createFile() const {
  FileRef fileref;
  fileref.logical_filename = util::Random::alphanumericString(32);
  fileref.absolute_path = FileUtil::joinPaths(
      basedir_,
      fileref.logical_filename);

  return fileref;
}

void FileRepository::listFiles(
    std::function<bool(const char*)> callback) const {
  FileUtil::ls(basedir_, callback);
}


}
}

