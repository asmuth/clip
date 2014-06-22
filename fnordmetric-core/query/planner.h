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

class SValue {
public:
  enum kSValueType {
    T_STRING,
    T_FLOAT,
    T_INTEGER
  };

  explicit SValue(int64_t integer_value) : type_(T_INTEGER) {
    data_.t_integer = integer_value;
  }

  explicit SValue(double float_value);
  explicit SValue(const char* str_value, size_t len, bool copy);
  SValue(const SValue& copy);
  SValue& operator=(const SValue& copy) = delete;

  ~SValue() {
  }

  static SValue* fromToken(const Token* token) {
    switch (token->getType()) {

      case Token::T_NUMERIC: {
        return new SValue((int64_t) 123);
      }

      default:
        return nullptr;

    }
  }

  kSValueType getType() const {
    return type_;
  }

  int64_t getInteger() const {
    assert(type_ == T_INTEGER);
    return data_.t_integer;
  }

protected:
  kSValueType type_;
  union {
    int64_t t_integer;
    double t_float;
    struct {
      const char* begin;
      const char* end;
      bool owned;
    } t_string;
  } data_;
};

class Executable {
public:
  virtual ~Executable() {}
  virtual void execute() = 0;
protected:

  SValue* expr(ASTNode* e) {
    printf("execute expression %p\n", e);

    e->debugPrint(2);
    switch (e->getType()) {
      case ASTNode::T_ADD_EXPR:
        return addExpr(e);

      case ASTNode::T_MUL_EXPR:
        return mulExpr(e);

      case ASTNode::T_LITERAL:
        assert(e->getToken() != nullptr);
        return SValue::fromToken(e->getToken());

      default:
        assert(0); // FIXPAUL
    };
  }

  SValue* addExpr(ASTNode* e) {
    auto args = e->getChildren();
    assert(args.size() == 2);

    std::unique_ptr<SValue> lhs(expr(args[0]));
    std::unique_ptr<SValue> rhs(expr(args[1]));

    if (lhs.get() == nullptr || rhs.get() == nullptr) {
      return nullptr;
    }

    SValue* result = nullptr;
    switch(lhs->getType()) {

      case SValue::T_INTEGER: {
        switch(rhs->getType()) {
          case SValue::T_INTEGER: {
            result = new SValue(lhs->getInteger() + rhs->getInteger());
          }
        }
      }
    }

    if (result == nullptr) {
      assert(0); // FIXPAUL
    } else {
      return result;
    }
  }

  SValue* mulExpr(ASTNode* e) {
    auto args = e->getChildren();
    assert(args.size() == 2);

    std::unique_ptr<SValue> lhs(expr(args[0]));
    std::unique_ptr<SValue> rhs(expr(args[1]));

    if (lhs.get() == nullptr || rhs.get() == nullptr) {
      return nullptr;
    }

    SValue* result = nullptr;
    switch(lhs->getType()) {

      case SValue::T_INTEGER: {
        switch(rhs->getType()) {
          case SValue::T_INTEGER: {
            result = new SValue(lhs->getInteger() * rhs->getInteger());
          }
        }
      }
    }

    if (result == nullptr) {
      assert(0); // FIXPAUL
    } else {
      return result;
    }
  }



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
    for (const auto& col : columns_) {
      std::unique_ptr<SValue> value(expr(col.second));

      printf("execute col=%s=%i\n",col.first.c_str(), value->getInteger());
    }
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
