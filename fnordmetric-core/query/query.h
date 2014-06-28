/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_QUERY_H
#define _FNORDMETRIC_QUERY_QUERY_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include "resultlist.h"

namespace fnordmetric {
namespace query {
class TableRepository;

class Query {
public:
  explicit Query(const char* query_string, TableRepository* repo);
  Query(const Query& copy) = delete;
  Query& operator=(const Query& copy) = delete;
  Query(Query&& move);

  bool execute();
  const ResultList& getResults(size_t statement_index);

protected:

  bool addSelectStatement(ASTNode* statement, TableRepository* repo);

  std::vector<std::unique_ptr<Executable>> statements_;
  //std::vector<std::unique_ptr<Drawable>> drawables_;
  std::vector<ResultList> results_;
};

}
}
#endif
