/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_TABLELESSSELECT_H
#define _FNORDMETRIC_QUERY_TABLELESSSELECT_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"
#include "executable.h"

namespace fnordmetric {
namespace query {

class TablelessSelect : public Executable {
public:

  TablelessSelect(ASTNode* select_list) {
    for (auto col : select_list->getChildren()) {
      assert(*col == ASTNode::T_DERIVED_COLUMN); // FIXPAUL
      auto derived = col->getChildren();
      assert(derived.size() == 2); // FIXPAUL

      if (derived.size() == 2) {
        assert(*derived[1] == ASTNode::T_COLUMN_NAME); // FIXPAUL
        auto colname_token = derived[1]->getToken();
        assert(colname_token && *colname_token == Token::T_IDENTIFIER);

        columns_.push_back(std::make_pair(
            colname_token->getString(),
            derived[0]->deepCopy()));
      }
    }
  }

  void execute() override {
    for (const auto& col : columns_) {
      std::unique_ptr<SValue> value(expr(col.second));

      printf("execute col=%s=%i\n",col.first.c_str(), value->getInteger());
    }
  }


protected:
  std::vector<std::pair<std::string, ASTNode*>> columns_;
};

}
}
#endif
