/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_FNV_H
#define _FNORDMETRIC_FNV_H

#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {
namespace util {

/**
 * This implements the FNV1a (Fowler–Noll–Vo) hash function
 *   see http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function
 */
template<typename T>
class FNV {
public:

  FNV();
  FNV(T basis, T prime) : basis_(basis), prime_(prime) {}

  T hash(const uint8_t* data, size_t len) const {
    T memory = basis_;

    for (size_t i = 0; i < len; ++i) {
      memory ^= data[i];
      memory *= prime_;
    }

    return memory;
  }

  inline T hash(const void* data, size_t len) const {
    return hash(static_cast<const uint8_t*>(data), len);
  }

  inline T hash(const std::string& data) const {
    return hash(reinterpret_cast<const uint8_t*>(data.c_str()), data.length());
  }

protected:
  T basis_;
  T prime_;
};

}
}

#endif
