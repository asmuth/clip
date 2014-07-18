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
#include "drawable.h"
#include "rendertarget.h"

namespace fnordmetric {
namespace query {
class TableRepository;
class DrawStatement;
}

class Query {
public:
  explicit Query(const char* query_string, query::TableRepository* repo);
  Query(const Query& copy) = delete;
  Query& operator=(const Query& copy) = delete;
  Query(Query&& move);

  bool execute(TableRenderTarget* target);
  bool execute(ChartRenderTarget* target);

protected:
  bool addStatement(
      query::ASTNode* statement,
      query::TableRepository* repo);

  Drawable* makeDrawable(query::DrawStatement* stmt);
  std::vector<std::unique_ptr<query::Executable>> statements_;
};

}
#endif
