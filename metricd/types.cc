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
#include <string.h>
#include "metricd/types.h"

namespace fnordmetric {

void tval_zero(tval_type type, void* reg, size_t reg_len) {
  switch (type) {

    case tval_type::UINT64: {
      assert(reg_len == sizeof(uint64_t));
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

      default:
        return false;

  }
}

size_t tval_len(tval_type t) {
  switch (t) {
    case tval_type::UINT64: return sizeof(uint64_t);
    default: throw std::invalid_argument("type error");
  }
}

} // namespace fnordmetric

