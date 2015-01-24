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
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "fnord/base/buffer.h"
#include "fnord/base/exception.h"
#include "fnord/base/stringutil.h"
#include "fnord/base/io/fileutil.h"
#include "fnord/base/io/file.h"

namespace fnord {

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
  String p1_stripped = p1;
  fnord::StringUtil::stripTrailingSlashes(&p1_stripped);
  String p2_stripped = p2;
  fnord::StringUtil::stripTrailingSlashes(&p2_stripped);
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

void FileUtil::mv(const std::string& src, const std::string& dst) {
  if (::rename(src.c_str(), dst.c_str()) < 0) {
    RAISE_ERRNO(kIOError, "rename(%s, %s) failed", src.c_str(), dst.c_str());
  }
}

void FileUtil::truncate(const std::string& filename, size_t new_size) {
  if (::truncate(filename.c_str(), new_size) < 0) {
    RAISE_ERRNO(kIOError, "truncate(%s) failed", filename.c_str());
  }
}

Buffer FileUtil::read(const std::string& filename) {
  auto file = File::openFile(filename, File::O_READ);
  Buffer buf(file.size());
  file.read(&buf);
  return buf;
}

void FileUtil::write(const std::string& filename, const Buffer& data) {
  auto file = File::openFile(
      filename,
      File::O_WRITE | File::O_CREATEOROPEN | File::O_TRUNCATE);

  file.write(data);
}

void FileUtil::cp(const std::string& src, const std::string& destination) {
  RAISE(kNotYetImplementedError);
}

}
