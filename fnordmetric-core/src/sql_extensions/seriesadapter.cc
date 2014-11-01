/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/runtime/resultlist.h>
#include <fnordmetric/sql_extensions/seriesadapter.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {

AnySeriesAdapter::AnySeriesAdapter(
    int name_ind,
    int x_ind,
    int y_ind,
    int z_ind,
    ResultList* result_list) :
    name_ind_(name_ind),
    x_ind_(x_ind),
    y_ind_(y_ind),
    z_ind_(z_ind),
    result_list_(result_list) {}

void AnySeriesAdapter::applyProperties(
    SValue* row,
    int row_len,
    Series* series,
    Series::AnyPoint* point) {
  for (const auto& prop : prop_indexes_) {
    if (prop.second >= row_len) {
      RAISE(kRuntimeError, "invalid index for property");
    }

    series->setProperty(prop.first, point, row[prop.second].toString());
  }
}

void AnySeriesAdapter::copyToResultList(SValue* row, int row_len) {
  if (result_list_ != nullptr) {
    result_list_->nextRow(row, row_len);
  }
}

}
}
