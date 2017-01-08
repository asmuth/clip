/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
FnordMetricTopSeries.TableEventHandler = function(table) {
  'use strict';

  this.init = function() {
    table.onSort(onSortHandler)

    table.onClick(onClickHandler)

    table.onRightClick(onRightClickHandler)
  }

  function onSortHandler() {

  }

  function onClickHandler() {
    //navigate to series page
  }

  function onRightClickHandler() {
    //render context menu
  }
}

