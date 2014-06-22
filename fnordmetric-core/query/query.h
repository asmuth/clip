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
  static bool parse(
      const char* query_string,
      TableRepository* repo,
      std::vector<std::unique_ptr<Query>>* destination);

  explicit Query(std::unique_ptr<Executable>&& executable);
  Query(const Query& copy) = delete;
  Query& operator=(const Query& copy) = delete;
  Query(Query&& move);

  bool execute();
  const ResultList& getResults();

protected:
  std::unique_ptr<Executable> executable_;
  ResultList results_;
};

}
}
#endif
