/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_AXISSTATEMENT_H
#define _FNORDMETRIC_QUERY_AXISSTATEMENT_H
#include <stdlib.h>
#include <assert.h>
#include "compile.h"
#include "execute.h"
#include "executable.h"
#include "../ui/axisdefinition.h"

namespace fnordmetric {
namespace query {

class AxisStatement : public Executable {
public:

  AxisStatement() {}

  void execute() override {}

  bool nextRow(SValue* row, int row_len) override {
    return true;
  }

  size_t getNumCols() const override {
    return 0;
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

  template <typename T>
  void executeDrawable(T* drawable) {
    drawable->addAxis(ui::AxisDefinition::LEFT); // FIPXAUL
    drawable->addAxis(ui::AxisDefinition::BOTTOM); // FIPXAUL
  }

protected:
  std::vector<std::string> columns_;
};

}
}
#endif
