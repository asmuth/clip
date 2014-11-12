/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Laura Schlimmer
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
if (FnordMetric === undefined) {
  FnordMetric = {};
}

if (FnordMetric.views === undefined) {
  FnordMetric.views = {};
}

FnordMetric.util.DatePicker = function(elem, input) {
  var dp_widget = document.createElement("div");
  elem.appendChild(dp_widget);

  function init() {
    var now = new Date();
    var month = now.getMonth();
    var human_days = ["Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"];
    var table = document.createElement("table");
    var header = document.createElement("div");
    header.innerHTML = FnordMetric.util.getMonthStr(month);
    var tbl_header = document.createElement("tr");
    human_days.map(function(day) {
      var header_cell = document.createElement("th");
      header_cell.innerHTML = day;
      tbl_header.appendChild(header_cell);
    });

    table.appendChild(tbl_header);
    dp_widget.appendChild(header);
    dp_widget.appendChild(table);
  }




  input.addEventListener('focus', function() {
    console.log("open dateoicker");
    init()
  }, false);

  input.addEventListener('blur', function() {
    dp_widget.innerHTML = "";
  }, false);

};

