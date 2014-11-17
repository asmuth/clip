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

/* Displays a calendar from an input for selecting time and date*/
FnordMetric.util.DatePicker = function(elem, dp_input, viewport, callback) {
  var currMonth;
  var currYear;

  var dp_widget = document.createElement("div");
    dp_widget.className = "datepicker_widget";
    elem.appendChild(dp_widget);

  var currentTimestamp = new Date(
    parseInt(dp_input.getAttribute("id"), 10));


  function getDaysInMonth(year, month) {
    return (new Date(year, (month+1), 0).getDate());
  }

  function getFirstDayOfMonth(year, month) {
    return (new Date(year + "-" + (month+1) + "-01").getDay());
  }

  /*checks if the day is in this month or not
    day and first_day are numeric weekday description
    rowNum the actual row and date the actual date
  */
  function inThisMonth(day, first_day, date, daysInMonth, rowNum) {
    if (rowNum > 0 && rowNum < 4) {return true}
    if (rowNum == 0) {
      /* the calendar has the same number of rows for each month */
      if (first_day == 0) {
        return false;
      } else if (day < first_day) {
        return false;
      } else {
        return true;
      }
    } else if (rowNum >= 4) {
      if (date <= daysInMonth) {
        return true;
      } else {
        return false;
      }
    }
  }

  function isSelectable(

  /* generate html for time input and handle input */
  function renderTimeInput() {
    var currentMinutes =
      FnordMetric.util.appendLeadingZero(
        currentTimestamp.getMinutes());

    var currentHours =
      FnordMetric.util.appendLeadingZero(
        currentTimestamp.getHours());

    var input_container = document.createElement("div");
    input_container.className = "input_container";
    var separator = document.createElement("span");
    separator.innerHTML = ":";

    var hour_input = document.createElement("input");
    hour_input.placeholder = currentHours;

    var minute_input = document.createElement("input");
    minute_input.placeholder = currentMinutes;

    input_container.appendChild(hour_input);
    input_container.appendChild(separator);
    input_container.appendChild(minute_input);
    dp_widget.appendChild(input_container);

    hour_input.addEventListener('focus', function(e) {
      e.preventDefault();
      FnordMetric.util.validatedTimeInput(this, "hour");
    }, false);

    minute_input.addEventListener('focus', function(e) {
      e.preventDefault();
      FnordMetric.util.validatedTimeInput(this, "minute");
    }, false);
  }

  function resetCalendar(table) {
    console.log("reset Calendar");
    table.innerHTML = "";
    FnordMetric.util.removeIfChild(
      table, dp_widget);
  }

  function renderWeekHeader(table) {
    var header = document.createElement("tr");

    var dayNames = 
      ["Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"];

    dayNames.map(function(day) {
      var cell = document.createElement("th");
      cell.innerHTML = day;
      header.appendChild(cell);
    });

    table.appendChild(header);
  }

  function renderMonthHeader(year, month, table) {
    var year = parseInt(year, 10);
    var month = parseInt(month, 10);
    var row = document.createElement("tr");
    var cell = document.createElement("td");
    cell.colSpan = "7";

    var month_title = document.createElement("span");
    month_title.className = "datepicker_title";
    month_title.innerHTML =
      FnordMetric.util.getMonthStr(month) + " " + year;

    /* tooltip to select previous month */
    var prev_ttp = FnordMetric.createButton(
      "#", "month_ttp", "<i class='fa fa-chevron-left'></i>");

    /* tooltip to select next month */
    var next_ttp = FnordMetric.createButton(
      "#", "month_ttp", "<i class='fa fa-chevron-right'></i>");

    prev_ttp.addEventListener('click', function(e) {
      e.preventDefault();
      resetCalendar(table);
      year = (month == 0)? year-1 : year;
      month = (month + 11) % 12;
      renderCalendar(year, month);
    }, false);

    next_ttp.addEventListener('click', function(e) {
      e.preventDefault();
      resetCalendar(table);
      year = (month == 11)? year+1 : year;
      month = (month + 1) % 12;
      renderCalendar(year, month);
    }, false);

    cell.appendChild(prev_ttp);
    cell.appendChild(month_title);
    cell.appendChild(next_ttp);
    row.appendChild(cell);
    table.appendChild(row);
  }

  function handleDayLinks(link, date, month, year) {
    link.addEventListener('click', function(e) {
      console.log("click";
      console.log(date + month + year);
    }, false);
  }

  function renderWeeks(year, month, calendar) {
    var daysInMonth = getDaysInMonth(year, month);
    var fDay = getFirstDayOfMonth(year, month);
    var numRows = 6;
    var date = 1;

    /* generate the rows (that is the weeks) */
    for (var dprow = 0; dprow < numRows; dprow++) {
      var row = document.createElement("tr");
      for (day = 0; day < 7; day ++) {
        var cell = document.createElement("td");
        if (inThisMonth(day, fDay, date, daysInMonth, dprow)) {
          if (isSelectable(date, month, year)) {
            var link = FnordMetric.createButton(
              "#", undefined, date);
            cell.appendChild(link);
            handleDateLinks(link, date, month, year);
          } else {
            cell.innerHTML = date;
          }
          date++;
        }
        row.appendChild(cell);
      }
      calendar.appendChild(row);
    }
  }


  function renderCalendar(year, month) {
    console.log("render calendar " + year + month);
    var table = document.createElement("table");
    renderWeekHeader(table);
    renderMonthHeader(year, month, table);
    renderWeeks(year, month, table);
    dp_widget.appendChild(table);
  }

  function init() {
    var now = new Date();
    currYear = now.getFullYear();
    currMonth = now.getMonth();
    dp_widget.innerHTML = "";
    dp_widget.className += " active";
    renderTimeInput();
    renderCalendar(currYear, currMonth);
  }



  dp_input.addEventListener('focus', function(event) {
    init();
  }, false);




}

