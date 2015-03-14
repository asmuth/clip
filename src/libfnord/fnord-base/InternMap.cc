/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string.h>
#include "fnord-base/exception.h"
#include "fnord-base/InternMap.h"

namespace fnord {

void* InternMap::internString(const String& str) {
  std::unique_lock<std::mutex> lk(intern_map_mutex_);

  auto iter = intern_map_.find(str);
  if (iter != intern_map_.end()) {
    return iter->second;
  }

  char* istr_raw = (char*) malloc(sizeof(InternedStringHeader) + str.length());
  if (istr_raw == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }

  InternedStringHeader* istr = (InternedStringHeader*) istr_raw;
  istr->magic = kMagic;
  istr->size = str.length();
  memcpy(istr_raw + sizeof(InternedStringHeader), str.c_str(), str.length());
  intern_map_[str] = istr_raw;
  return istr_raw;
}

String InternMap::getString(const void* interned) {
  if (((InternedStringHeader*) interned)->magic != kMagic) {
    RAISEF(kRuntimeError, "invalid interned string: $0", interned);
  }

  auto data = ((char*) interned) + sizeof(InternedStringHeader);
  auto size = ((InternedStringHeader*) interned)->size;

  return std::string(data, size);
}

} // namespace fnord
