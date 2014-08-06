/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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
