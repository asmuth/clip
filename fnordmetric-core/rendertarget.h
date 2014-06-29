/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_RENDERTARGET_H
#define _FNORDMETRIC_RENDERTARGET_H
#include <stdlib.h>
#include <vector>
#include <string>
#include "query/executable.h"
#include "query/svalue.h"

namespace fnordmetric {

class TableRenderTarget : public query::RowSink {
public:
  virtual void addHeader(const std::vector<std::string>& columns) = 0;
  virtual void addRow() = 0;
  virtual void addColumn(const std::string& value) = 0;

  bool nextRow(query::SValue* row, int row_len) {
    addRow();
    for (int i = 0; i < row_len; ++i) {
      addColumn(row[i].toString());
    }
    return true;
  }

};

class ChartRenderTarget {
public:

  virtual void beginChart(int width, int height) = 0;
  virtual void finishChart() = 0;

  virtual void beginGroup(const std::string& class_name) = 0;
  virtual void finishGroup() = 0;

  virtual void drawRect(
      double x,
      double y,
      double width,
      double height) = 0;

  virtual void drawLine(
      double x1,
      double y1,
      double x2,
      double y2,
      const std::string& class_name) = 0;

};


}
#endif
