/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_DRAWSTATEMENT_H
#define _FNORDMETRIC_QUERY_DRAWSTATEMENT_H
#include <stdlib.h>
#include <assert.h>
#include "compile.h"
#include "execute.h"
#include "../drawable.h"

namespace fnordmetric {
namespace query {

class DrawStatement : public Executable {
public:

  enum kDrawStatementType {
    T_BAR_CHART,
    T_LINE_CHART,
    T_AREA_CHART
  };

  DrawStatement(
      kDrawStatementType type) :
      type_(type) {}

  void execute() override {
    // FIXPAUL execute prop list
  }

  bool nextRow(SValue* row, int row_len) override {
    assert(0);
  }

  size_t getNumCols() const override {
    return 0;
  }

  const std::vector<std::string>& getColumns() const override {
    static std::vector<std::string> cols;
    return cols;
  }

  kDrawStatementType getType() const {
    return type_;
  }

protected:
  kDrawStatementType type_;
};

}
}
#endif
