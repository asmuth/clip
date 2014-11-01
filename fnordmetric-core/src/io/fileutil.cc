/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <dirent.h>
#include <fnordmetric/io/fileutil.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/util/stringutil.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

namespace fnord {
namespace io {

void FileUtil::mkdir(const std::string& dirname) {
  if (::mkdir(dirname.c_str(), S_IRWXU) != 0) {
    RAISE_ERRNO(kIOError, "mkdir('%s') failed", dirname.c_str());
  }
}

bool FileUtil::exists(const std::string& filename) {
  struct stat fstat;

  if (stat(filename.c_str(), &fstat) < 0) {
    if (errno == ENOENT) {
      return false;
    }

    RAISE_ERRNO(kIOError, "fstat('%s') failed", filename.c_str());
  }

  return true;
}

bool FileUtil::isDirectory(const std::string& filename) {
  struct stat fstat;

  if (stat(filename.c_str(), &fstat) < 0) {
    RAISE_ERRNO(kIOError, "fstat('%s') failed", filename.c_str());
  }

  return S_ISDIR(fstat.st_mode);
}

/* The mkdir_p method was adapted from bash 4.1 */
void FileUtil::mkdir_p(const std::string& dirname) {
  char const* begin = dirname.c_str();
  char const* cur = begin;

  if (exists(dirname)) {
    if (isDirectory(dirname)) {
      return;
    } else {
      RAISE(
          kIOError,
          "file '%s' exists but is not a directory",
          dirname.c_str());
    }
  }

  for (cur = begin; *cur == '/'; ++cur);

  while ((cur = strchr(cur, '/'))) {
    std::string path(begin, cur);
    cur++;

    if (exists(path)) {
      if (isDirectory(path)) {
        continue;
      } else {
        RAISE(
            kIOError,
            "file '%s' exists but is not a directory",
            path.c_str());
      }
    }

    mkdir(path);
  }

  mkdir(dirname);
}

std::string FileUtil::joinPaths(const std::string& p1, const std::string p2) {
  auto p1_stripped = p1;
  util::StringUtil::stripTrailingSlashes(&p1_stripped);
  auto p2_stripped = p2;
  util::StringUtil::stripTrailingSlashes(&p2_stripped);
  return p1_stripped + "/" + p2_stripped;
}

void FileUtil::ls(
    const std::string& dirname,
    std::function<bool(const std::string&)> callback) {
  if (exists(dirname)) {
    if (!isDirectory(dirname)) {
      RAISE(
          kIOError,
          "file '%s' exists but is not a directory",
          dirname.c_str());
    }
  } else {
    RAISE(
        kIOError,
        "file '%s' does not exist",
        dirname.c_str());
  }

  auto dir = opendir(dirname.c_str());

  if (dir == nullptr) {
    RAISE_ERRNO("opendir(%s) failed", dirname.c_str());
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
#if defined(__APPLE__)
    size_t namlen = entry->d_namlen;
#else
    size_t namlen = strlen(entry->d_name);
#endif
    if (namlen < 1 || *entry->d_name == '.') {
      continue;
    }

    if (!callback(std::string(entry->d_name, namlen))) {
      break;
    }
  }

  closedir(dir);
}

void FileUtil::rm(const std::string& filename) {
  unlink(filename.c_str());
}


}
}
