/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_DOCUMENTKEY_H
#define _FNORDMETRIC_DOCUMENTKEY_H
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>

namespace fnordmetric {

/**
 */
class DocumentKey {
public:
  explicit DocumentKey(const std::string& key);
  explicit DocumentKey(uint64_t key);
  DocumentKey(const DocumentKey& copy);
  DocumentKey& operator==(const DocumentKey& copy) = delete;
  ~DocumentKey();
  bool isIntKey() const;
  bool isStringKey() const;
  uint64_t getIntKey() const;
  const std::string* getStringKey() const;
protected:
  bool is_string_;
  union {
    uint64_t int_key;
    std::string* string_key;
  } key_;
};

}
#endif
