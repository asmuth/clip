/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_H
#define _FNORDMETRIC_QUERY_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnordmetric {
namespace query {
class TableRepository;
class DrawStatement;
class ASTNode;
class Executable;
class AbstractResultList;

class Query {
public:
  explicit Query(const char* query_string, TableRepository* repo);
  Query(const Query& copy) = delete;
  Query& operator=(const Query& copy) = delete;
  Query(Query&& move);

  bool execute(AbstractResultList* target);

protected:
  bool addStatement(ASTNode* statement, TableRepository* repo);

  //Drawable* makeDrawable(query::DrawStatement* stmt);
  std::vector<std::unique_ptr<Executable>> statements_;
};

}
}
#endif
