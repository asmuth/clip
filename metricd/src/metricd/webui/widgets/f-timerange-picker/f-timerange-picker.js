/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FM = this.FM || {};

FM.TimeRangePickerComponent = function(elem) {
  'use strict';

  /** default config **/
  var config = {
    range: 5 * dateUtil.kMillisPerMinute, //5 minutes
  }

  var timerange = {
    timezone: 'utc'
  };
  timerange.end = Date.now();
  timerange.start = timerange.end - config.range;

  var widget;
  var on_submit_callbacks = [];

  /**
    * Set the initial values
    * @param init_timerange (Object) determines the initial values for
    * start, end and timezone
  **/
  this.initialize = function(init_timerange) {
    var tpl = templateUtil.getTemplate("f-timerange-picker-tpl");
    elem.appendChild(tpl);

    if (init_timerange.timezone) {
      timerange.timezone = init_timerange.timezone;
    }

    if (init_timerange.start && init_timerange.end) {
      timerange.start = init_timerange.start;
      timerange.end = init_timerange.end;
    }

    initializeWidget();
    watchTimerangeMover();
    watchInputClick();

    updateInputValue();
  }

  this.onSubmit = function(callback_fn) {
    on_submit_callbacks.push(callback_fn);
  }

/******************************** private *************************************/

  var initializeWidget = function() {
    widget = new TimeRangePickerWidget(
        timerange,
        elem.querySelector(".widget"));

    widget.setSubmitCallback(function(new_timerange) {
      timerange.start = new_timerange.start;
      timerange.end = new_timerange.end;
      timerange.timezone = new_timerange.timezone;

      updateInputValue();
      callSubmitCallbacks();
    });

    elem.addEventListener("click", function(e) {
      e.stopPropagation();
    }, false);
  }

  var watchTimerangeMover = function() {
    elem.querySelector(".mover.prev").addEventListener("click", function(e) {
      if (!this.classList.contains("disabled")) {
        moveTimerange(config.range * -1);
      }
    }, false);

    elem.querySelector(".mover.next").addEventListener("click", function(e) {
      if (!this.classList.contains("disabled")) {
        moveTimerange(config.range);
      }
    }, false);
  }

  var watchInputClick = function() {
    elem.querySelector(".date_field").addEventListener("click", function(e) {
      widget.toggleVisibility(timerange);
    }, false);
  }

  var moveTimerange = function(range) {
    timerange.start += range;
    timerange.end += range;

    updateInputValue();
    callSubmitCallbacks();
  }

  var updateInputValue = function() {
    elem.querySelector(".date_field .date_value").innerHTML =
        dateUtil.formatDateTime(timerange.start, timerange.timezone) +
        " - " +
        dateUtil.formatDateTime(timerange.end, timerange.timezone);


    if (timerange.end + config.range >= Date.now()) {
      elem.querySelector(".mover.next").classList.add("disabled");
    } else {
      elem.querySelector(".mover.next").classList.remove("disabled");
    }
  }

  function callSubmitCallbacks() {
    on_submit_callbacks.forEach(function(fn) {
      fn({
        start: timerange.start,
        end: timerange.end,
        timezone: timerange.timezone
      });
    });
  }
}


