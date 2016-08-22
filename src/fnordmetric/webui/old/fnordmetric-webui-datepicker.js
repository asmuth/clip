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

 /* generate html for time input and handle input */
FnordMetric.util.timeInput = function(selectedTimestamp, elem, callback) {
  var selectedMinutes =
      FnordMetric.util.appendLeadingZero(
        selectedTimestamp.getMinutes());

    var selectedHours =
      FnordMetric.util.appendLeadingZero(
        selectedTimestamp.getHours());

    var input_container = document.createElement("div");
    input_container.className = "input_container";
    var separator = document.createElement("span");
    separator.innerHTML = ":";

    var hour_input = document.createElement("input");
    hour_input.value = selectedHours;

    var minute_input = document.createElement("input");
    minute_input.value = selectedMinutes;

  function render() {
    input_container.appendChild(hour_input);
    input_container.appendChild(separator);
    input_container.appendChild(minute_input);
    elem.appendChild(input_container);

    hour_input.addEventListener('focus', function(e) {
      e.preventDefault();
      FnordMetric.util.validatedTimeInput(this, "hours", callback);
    }, false);

    minute_input.addEventListener('focus', function(e) {
      e.preventDefault();
      FnordMetric.util.validatedTimeInput(this, "minutes", callback);
    }, false);


  }

  function getValues() {
    var hours = hour_input.value.length > 0 ?
      hour_input.value : selectedHours;
    var minutes = minute_input.value.length > 0 ?
      minute_input.value : selectedMinutes;
    return {
      "hours" : hours,
      "minutes" : minutes
    }
  }

  return {
    "render" : render,
    "getValues" : getValues,
  }
}


/* Displays a calendar from an input for selecting time and date*/
FnordMetric.util.DatePicker = function(elem, dp_input, viewport, callback) {
  /* current date */
  var currMonth;
  var currYear;
  var currDate;

  /* in datepicker selected date */
  var selectedTimestamp;
  var selectedYear;
  var selectedMonth;
  var selectedDate;

  var timeInput;
  var dp_widget = document.createElement("div");
  dp_widget.className = "datepicker_widget";
  elem.appendChild(dp_widget);


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

  function isSelectable(date, month, year) {
    if (month == currMonth) {
      return date <= currDate;
    }
    return (year <= currYear && month < currMonth);
  }

  /* if date is selectable */
  function getCellName(date, month, year) {
    var name = "";
    /* date is today */
    if (date == currDate && month == currMonth && year == currYear) {
      name = "highlight_border";
    }
    /* date is selected date */
    if (date == selectedDate && 
        month == selectedMonth && 
        year == selectedYear) {
          name += " highlight";
    }
    return name;
  }


  function onSelect(date, month, year) {
    /* fallback for time inputs */
    var date = (date == undefined) ? 
      selectedDate : date;
    var month = (month == undefined)?
      selectedMonth : month;
    var year = (year == undefined) ?
      selectedYear : year;
    var inputs = timeInput.getValues();
    var hours = inputs.hours;
    var minutes = inputs.minutes;
    var ts = new Date(year, month, date, hours, minutes).getTime();
    if (ts <= Date.now() ) {
      dp_input.value =
        FnordMetric.util.appendLeadingZero(month+1) + 
        "/" +
        FnordMetric.util.appendLeadingZero(date) + 
        "/" + year + "  " + 
        FnordMetric.util.appendLeadingZero(hours) +
        ":" + 
        FnordMetric.util.appendLeadingZero(minutes);

      callback(ts);
      dp_input.setAttribute("id", ts);
      resetDatepicker();
    }
  }


  /* sets event listener for selectable dates */
  function handleDateLinks(link, date, month, year) {
    link.addEventListener('click', function(e) {
      e.preventDefault();
      onSelect(date, month, year);
    }, false);
  }

  function resetDatepicker() {
    dp_widget.innerHTML = "";
    dp_widget.className = "datepicker_widget";
  }

  function resetCalendar(table) {
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
      FnordMetric.util.getHumanMonth(month, "long") + " " + year;

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
            cell.className = getCellName(date, month, year);
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
    currDate = now.getDate();

    selectedTimestamp = new Date(
      parseInt(dp_input.getAttribute("id"), 10));
    selectedYear = selectedTimestamp.getFullYear();
    selectedMonth = selectedTimestamp.getMonth();
    selectedDate = selectedTimestamp.getDate();

    dp_widget.innerHTML = "";
    dp_widget.className += " active";
    timeInput = FnordMetric.util.timeInput(
      selectedTimestamp, dp_widget, onSelect);
    timeInput.render();
    renderCalendar(currYear, currMonth);
  }

  dp_input.addEventListener('focus', function(event) {
    init();
  }, false);

  document.addEventListener('click', function() {
    resetDatepicker();
  }, false);


  elem.addEventListener('click', function(e) {
    e.stopPropagation();
  });
}

