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
  dp_widget.className = "datepicker_widget";
  elem.appendChild(dp_widget);

  var human_days = ["Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"];

  function resetDatepicker(day, month, year) {
    dp_widget.innerHTML = "";
    input.value = month + "/" + day + "/" + year;
  }

  function init(month, year) {
    var first_day = new Date(year + "-" + (month+1) + "-01").getDay();
    /* Mo = 1, ... , Su = 7 */
    first_day = (first_day === 0)? 7 : first_day-1;
    console.log("first day " + first_day);
    var num_days = new Date(year, (month+1), 0).getDate();
    var table = document.createElement("table");

    var month_header = document.createElement("tr");
    var prev_ttp = FnordMetric.createButton(
      "#", undefined, "<i class='fa fa-chevron-left'></i>");
    var next_ttp = FnordMetric.createButton(
      "#", undefined, "<i class='fa fa-chevron-right'></i>");
    var month_title = document.createElement("span");
    month_title.innerHTML =  FnordMetric.util.getMonthStr(month);
    var month_cell = document.createElement("td");
    month_cell.colSpan = "7";
    month_cell.appendChild(prev_ttp);
    month_cell.appendChild(month_title);
    month_cell.appendChild(next_ttp);
    month_header.appendChild(month_cell);


    var day_header = document.createElement("tr");
    human_days.map(function(day) {
      var header_cell = document.createElement("th");
      header_cell.innerHTML = day;
      day_header.appendChild(header_cell);
    });

    var day = 1;
    var rows = 0;
    var first_row = document.createElement("tr");
    rows++;
    for (var i = 0; i < 7; i++) { 
      var cell = document.createElement("td");
      if (i < first_day) {
        cell.innerHTML = "";
      } else {
        var link = FnordMetric.createButton(
          "#", undefined, day);
        link.addEventListener('click', function(e) {
          e.preventDefault();
          resetDatepicker(this.innerText, month, year);
        }, false);

        cell.appendChild(link);
        day++;
      }
      first_row.appendChild(cell);
    }

    table.appendChild(day_header);
    table.appendChild(month_header);
    table.appendChild(first_row);

    while (rows < 6) {
      var row = document.createElement("tr");
      rows++;
      for (var i = 0; i < 7 && day <= num_days; i++) {
        var cell = document.createElement("td");
        var link = FnordMetric.createButton(
          "#", undefined, day);
        link.addEventListener('click', function(e) {
          e.preventDefault();
          resetDatepicker(this.innerText, month, year);
        }, false);

        cell.appendChild(link);
        row.appendChild(cell);
        day++;
      }
      table.appendChild(row);
    }

    if (rows < 6) {
      var last_row = document.createElement("tr");
      for (var i = 0; i < 7; i++) {
        var cell = document.createElement("td");
        if (day <= num_days) {
          var link = FnordMetric.createButton(
            "#", undefined, day);
          link.addEventListener('click', function(e) {
            e.preventDefault();
            resetDatepicker(this.innerText, month, year);
          }, false);

          cell.appendChild(link);
          day++;
        } else {
          cell.innerHTML = "";
        }
        last_row.appendChild(cell);
      }
      table.appendChild(last_row);
    }

    dp_widget.appendChild(table);
  }




  input.addEventListener('focus', function() {
    console.log("open dateoicker");
    dp_widget.innerHTML = "";
    var now = new Date();
    var month = now.getMonth();
    var year = now.getFullYear();
    init(month, year)
  }, false);

  input.addEventListener('blur', function() {
    //dp_widget.innerHTML = "";
  }, false);

};

