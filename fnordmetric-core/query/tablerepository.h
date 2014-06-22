/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_TABLEREPOSITORY_H
#define _FNORDMETRIC_QUERY_TABLEREPOSITORY_H
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <memory>
#include "tableref.h"

namespace fnordmetric {
namespace query {

class TableRepository {
public:

  TableRef* getTableRef(const std::string& table_name) {
    auto iter = table_refs_.find(table_name);

    if (iter == table_refs_.end()) {
      return nullptr;
    } else {
      return iter->second.get();
    }
  }

  void addTableRef(
      const std::string& table_name,
      std::unique_ptr<TableRef>&& table_ref) {
    table_refs_[table_name] = std::move(table_ref);
  }

protected:
  std::unordered_map<std::string, std::unique_ptr<TableRef>> table_refs_;
};

}
}
#endif
