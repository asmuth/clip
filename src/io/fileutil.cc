/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/io/fileutil.h>
#include <fnordmetric/util/runtimeexception.h>
#include <sys/stat.h>


namespace fnord {
namespace io {

void FileUtil::mkdir(const std::string dirname) {
  if (::mkdir(dirname.c_str(), S_IRWXU) != 0) {
    RAISE_ERRNO(kIOError, "mkdir('%s') failed", dirname.c_str());
  }
}

void FileUtil::mkdir_p(const std::string dirname) {
  RAISE(kNotYetImplementedError, "FileUtil::mkdir_p is not yet implemented");
}

}
}
