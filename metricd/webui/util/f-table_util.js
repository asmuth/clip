/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *   Copyright (c) 2016 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
fTableUtil = this.fTableUtil || {};

fTableUtil.buildRows = function(column_list, rows_arr) {
  var rows = [];
  rows_arr.forEach(function(row) {
    var cells = {};
    for (var i = 0; i < row.length; i++) {
      cells[column_list[i]] = {
        value: row[i]
      }
    }
    rows.push({"cells": cells});
  });

  return rows;
}
