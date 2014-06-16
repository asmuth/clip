/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
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
