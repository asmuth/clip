/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_FFS_OBJECTREF_H
#define _FNORDMETRIC_FFS_OBJECTREF_H
#include <fnordmetric/util/runtimeexception.h>
#include "object.h"
#include <fnordmetric/io/pagemanager.h>

namespace fnordmetric {
namespace ffs {

template <typename ObjectType>
class ObjectRef {
public:
  ObjectRef() = delete;
  ObjectRef(const ObjectRef<ObjectType>& other) = delete;
  ObjectRef<ObjectType>& operator=(const ObjectRef<ObjectType>& other) = delete;

  ObjectRef(ObjectType* obj) : obj_(obj) {
    obj_->incrRefcount();
  }

  ObjectRef(ObjectRef<ObjectType>&& other) {
    obj_ = other.obj_;
    other.obj_ = nullptr;
  }

  ~ObjectRef() {
    if (obj_ != nullptr) {
      obj_->decrRefcount();
    }
  }

  void close() {
    if (obj_ == nullptr) {
      RAISE(kIllegalStateError, "alread closed");
    } else {
      obj_->decrRefcount();
      obj_ = nullptr;
    }
  }

protected:
  ObjectType* obj_;
};

}
}
#endif
