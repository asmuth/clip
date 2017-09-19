/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string>

class SHA1Hash {
  friend class SHA1;
public:
  static const size_t kSize = 20;
  struct DeferInitialization {};

  /**
   * Parse a SHA1Hash from a hex encoded string
   */
  static bool fromHexString(const std::string& str, SHA1Hash* hash);

  /**
   * Creates a new zero-initialized SHA1 hash
   */
  SHA1Hash();

  /**
   * Creates an unitialized SHA1 hash. The initial value is undefined.
   */
  SHA1Hash(DeferInitialization);

  /**
   * Creates a new SHA1 hash from an already computed hash. size must be 20
   * bytes.
   */
  SHA1Hash(const void* data, size_t size);

  bool operator==(const SHA1Hash& other) const;
  bool operator!=(const SHA1Hash& other) const;
  bool operator<(const SHA1Hash& other) const;
  bool operator>(const SHA1Hash& other) const;

  int compare(const SHA1Hash& other) const;
  int compare(const void* other) const;

  inline const void* data() const {
    return hash;
  }

  inline void* mutableData() {
    return hash;
  }

  inline size_t size() const {
    return sizeof(hash);
  }

  std::string toString() const;

protected:
  uint8_t hash[kSize];
};

class SHA1 {
public:

  static SHA1Hash compute(const std::string& data);
  static void compute(const std::string& data, SHA1Hash* out);

  static SHA1Hash compute(const void* data, size_t size);
  static void compute(const void* data, size_t size, SHA1Hash* out);

  static int compare(const SHA1Hash& a, const SHA1Hash& b);
  static int compare(const void* a, const void* b);

};

namespace std {
template<>
struct hash<SHA1Hash> {
  size_t operator()(SHA1Hash const& val) const {
    static_assert(
        sizeof(size_t) < SHA1Hash::kSize,
        "sizeof(size_t must be <20 bytes");

    return *((const size_t*) val.data());
  }
};
}

