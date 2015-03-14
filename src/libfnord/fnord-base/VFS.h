/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_VFS_H_
#define _FNORD_VFS_H_
#include <fnord-base/stdtypes.h>
#include <fnord-base/autoref.h>

namespace fnord {

class VFSFile : public RefCounted {
public:
  virtual ~VFSFile() {}

  virtual size_t size() const = 0;
  virtual void* data() const = 0;

  template <typename T>
  inline T* structAt(size_t pos) const {
#ifndef FNORD_NODEBUG
    if (pos >= size()) {
      RAISE(kIndexError, "position out of bounds");
    }
#endif

    return (T*) (((char *) data()) + pos);
  }

};

class VFS {
public:
  virtual ~VFS() {}
  virtual RefPtr<VFSFile> openFile(const String& filename) = 0;
};

class WhitelistVFS : public VFS {
public:
  RefPtr<VFSFile> openFile(const String& filename) override;
  void registerFile(const String vfs_path, const String& real_path);
protected:
  HashMap<String, String> whitelist_;
};

}
#endif
