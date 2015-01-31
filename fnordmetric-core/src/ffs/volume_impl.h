/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_FFS_VOLUME_IMPL_H_
#define _FNORDMETRIC_FFS_VOLUME_IMPL_H_
#include <fnord-base/exception.h>

namespace fnordmetric {
namespace ffs {

template <typename ObjectType>
ObjectRef<ObjectType> Volume::insert(const std::string& object_key) {
  std::lock_guard<std::mutex> lock(live_objects_mutex_);

  /* FIXPAUL ensure it doesn't contain the elem */

  if (live_objects_.find(object_key) != live_objects_.end()) {
    RAISE(
        kIllegalStateError,
        "can't insert object because it already exists and is currently "
        "opened");
  }

  auto obj = new ObjectType(page_manager_);
  live_objects_.emplace(object_key, obj);
  return ObjectRef<ObjectType>(obj);
}

}
}
#endif
