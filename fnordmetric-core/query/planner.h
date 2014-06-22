/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_PLANER_H
#define _FNORDMETRIC_QUERY_PLANER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"

namespace fnordmetric {
namespace query {

class Executable {
public:
  virtual ~Executable() {}
  virtual void execute() = 0;
};

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
    printf("execute!\n");
  }
protected:
  std::vector<std::pair<std::string, ASTNode*>> columns_;
};

class Planner {
public:

  Planner(ASTNode* select_statement);
  Planner(const Planner& copy) = delete;
  Planner& operator=(const Planner& copy) = delete;

  std::unique_ptr<Executable> getExecutable() {
    assert(executable_ != nullptr);
    auto ptr = std::unique_ptr<Executable>(executable_);
    executable_ = nullptr;
    return ptr;
  }

protected:

  Executable* plan(ASTNode*);
  Executable* planTablelessSelect(ASTNode*);
  Executable* executable_;
};

}
}
#endif
