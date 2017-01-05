/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdexcept>
#include <assert.h>
#include <string>
#include <string.h>
#include <limits>
#include "metricd/types.h"

namespace fnordmetric {

tval_autoref::tval_autoref() {
  val.data = nullptr;
  val.len = 0;
}

tval_autoref::tval_autoref(tval_type type) {
  tval_alloc(&val, type);
  tval_zero(val.type, val.data, val.len);
}

tval_autoref::tval_autoref(tval_autoref&& other) {
  val.type = other.val.type;
  val.data = other.val.data;
  val.len = other.val.len;

  other.val.data = nullptr;
  other.val.len = 0;
}

tval_autoref::~tval_autoref() {
  if (val.data) {
    tval_free(&val);
  }
}

tval_autoref& tval_autoref::operator=(tval_autoref&& other) {
  if (val.data) {
    tval_free(&val);
  }

  val.type = other.val.type;
  val.data = other.val.data;
  val.len = other.val.len;

  other.val.data = nullptr;
  other.val.len = 0;
  return *this;
}

void tval_zero(tval_type type, void* reg, size_t reg_len) {
  switch (type) {

    case tval_type::UINT64: {
      assert(reg_len == sizeof(uint64_t));
      memset(reg, 0, reg_len);
      return;
    }

    case tval_type::INT64: {
      assert(reg_len == sizeof(int64_t));
      memset(reg, 0, reg_len);
      return;
    }

    case tval_type::FLOAT64: {
      static_assert(
          std::numeric_limits<double>::is_iec559 &&
          sizeof(double) == sizeof(uint64_t),
          "non-compatible double implementation");

      assert(reg_len == sizeof(double));
      memset(reg, 0, reg_len);
      return;
    }

    default: throw std::invalid_argument("type error");
  }
}

void tval_add(
    tval_type type,
    void* reg,
    size_t reg_len,
    const void* op,
    size_t op_len) {
  switch (type) {

    case tval_type::UINT64: {
      assert(reg_len == sizeof(uint64_t));
      assert(op_len == sizeof(uint64_t));
      *((uint64_t*) reg) += *((const uint64_t*) op);
      return;
    }

    case tval_type::INT64: {
      assert(reg_len == sizeof(int64_t));
      assert(op_len == sizeof(int64_t));
      *((int64_t*) reg) += *((const int64_t*) op);
      return;
    }

    case tval_type::FLOAT64: {
      static_assert(
          std::numeric_limits<double>::is_iec559 &&
          sizeof(double) == sizeof(uint64_t),
          "non-compatible double implementation");

      assert(reg_len == sizeof(double));
      assert(op_len == sizeof(double));
      *((double*) reg) += *((const double*) op);
      return;
    }

    default: throw std::invalid_argument("type error");
  }
}

void tval_sub(
    tval_type type,
    void* reg,
    size_t reg_len,
    const void* op,
    size_t op_len) {
  switch (type) {

    case tval_type::UINT64: {
      assert(reg_len == sizeof(uint64_t));
      assert(op_len == sizeof(uint64_t));
      *((uint64_t*) reg) -= *((const uint64_t*) op);
      return;
    }

    case tval_type::INT64: {
      assert(reg_len == sizeof(int64_t));
      assert(op_len == sizeof(int64_t));
      *((int64_t*) reg) -= *((const int64_t*) op);
      return;
    }

    case tval_type::FLOAT64: {
      static_assert(
          std::numeric_limits<double>::is_iec559 &&
          sizeof(double) == sizeof(uint64_t),
          "non-compatible double implementation");

      assert(reg_len == sizeof(double));
      assert(op_len == sizeof(double));
      *((double*) reg) -= *((const double*) op);
      return;
    }

    default: throw std::invalid_argument("type error");
  }
}

int tval_cmp(
    tval_type type,
    void* left,
    size_t left_len,
    const void* right,
    size_t right_len) {
  switch (type) {

    case tval_type::UINT64: {
      assert(left_len == sizeof(uint64_t));
      assert(right_len == sizeof(uint64_t));
      if (*((const uint64_t*) left) < *((const uint64_t*) right)) {
        return -1;
      } else if (*((const uint64_t*) left) > *((const uint64_t*) right)) {
        return 1;
      } else {
        return 0;
      }
    }

    case tval_type::INT64: {
      assert(left_len == sizeof(int64_t));
      assert(right_len == sizeof(int64_t));
      if (*((const int64_t*) left) < *((const int64_t*) right)) {
        return -1;
      } else if (*((const int64_t*) left) > *((const int64_t*) right)) {
        return 1;
      } else {
        return 0;
      }
    }

    case tval_type::FLOAT64: {
      static_assert(
          std::numeric_limits<double>::is_iec559 &&
          sizeof(double) == sizeof(uint64_t),
          "non-compatible double implementation");

      assert(left_len == sizeof(double));
      assert(right_len == sizeof(double));

      if (*((const double*) left) < *((const double*) right)) {
        return -1;
      } else if (*((const double*) left) > *((const double*) right)) {
        return 1;
      } else {
        return 0;
      }
    }

    default: throw std::invalid_argument("type error");
  }
}

bool tval_fromstring(
    tval_type type,
    void* val_data,
    size_t val_len,
    const char* str,
    size_t str_len) {
  switch (type) {

    case tval_type::UINT64: {
      assert(val_len == sizeof(uint64_t));
      try {
        *((uint64_t*) val_data) = std::stoull(std::string(str, str_len));
        return true;
      } catch (...) {
        return false;
      }
    }

    case tval_type::INT64: {
      assert(val_len == sizeof(int64_t));
      try {
        *((int64_t*) val_data) = std::stoll(std::string(str, str_len));
        return true;
      } catch (...) {
        return false;
      }
    }

    case tval_type::FLOAT64: {
      static_assert(
          std::numeric_limits<double>::is_iec559 &&
          sizeof(double) == sizeof(uint64_t),
          "non-compatible double implementation");

      assert(val_len == sizeof(double));

      try {
        *((double*) val_data) = std::stod(std::string(str, str_len));
        return true;
      } catch (...) {
        return false;
      }
    }

    default:
      return false;

  }
}

bool tval_fromstring(
    tval_ref* val,
    const std::string& str) {
  return tval_fromstring(
      val->type,
      val->data,
      val->len,
      str.data(),
      str.size());
}

size_t tval_len(tval_type t) {
  static_assert(
      std::numeric_limits<double>::is_iec559 &&
      sizeof(double) == sizeof(uint64_t),
      "non-compatible double implementation");

  switch (t) {
    case tval_type::UINT64: return sizeof(uint64_t);
    case tval_type::INT64: return sizeof(int64_t);
    case tval_type::FLOAT64: return sizeof(double);
    default: throw std::invalid_argument("type error");
  }
}

void tval_alloc(tval_ref* ref, tval_type type) {
  ref->type = type;
  ref->len = tval_len(type);
  ref->data = malloc(ref->len);
}

void tval_free(tval_ref* ref) {
  free(ref->data);
}

} // namespace fnordmetric

