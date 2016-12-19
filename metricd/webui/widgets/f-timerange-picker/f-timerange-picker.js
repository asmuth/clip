/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

var TimeRangePickerComponent = function() {
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

  var this_;
  var widget;

  this.createdCallback = function() {
    var tpl = templateUtil.getTemplate("f-timerange-picker-tpl");
    this.appendChild(tpl);

    this_ = this;
  }

  /**
    * Set the initial values
    * @param init_timerange (Object) determines the initial values for
    * start, end and timezone
  **/
  this.initialize = function(init_timerange) {
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

  /**
    * Returns start and end of the selected timerange
  **/
  this.getTimerange = function() {
    return {
      start: timerange.start,
      end: timerange.end,
    }
  }

  this.getTimezone = function() {
    return timerange.timezone;
  }


/******************************** private *************************************/

  var initializeWidget = function() {
    widget = new TimeRangePickerWidget(
        timerange,
        this_.querySelector(".widget"));

    widget.setSubmitCallback(function(new_timerange) {
      timerange.start = new_timerange.start;
      timerange.end = new_timerange.end;
      timerange.timezone = new_timerange.timezone;

      updateInputValue();
      fireSubmitEvent();
    });

    document.addEventListener("click", function(e) {
      widget.close();
    }, false);

    this_.addEventListener("click", function(e) {
      e.stopPropagation();
    }, false);
  }

  var watchTimerangeMover = function() {
    this_.querySelector(".arrow_left").addEventListener("click", function(e) {
      if (!this.classList.contains("disabled")) {
        moveTimerange(config.range * -1);
      }
    }, false);

    this_.querySelector(".arrow_right").addEventListener("click", function(e) {
      if (!this.classList.contains("disabled")) {
        moveTimerange(config.range);
      }
    }, false);
  }

  var watchInputClick = function() {
    this_.querySelector("input").addEventListener("click", function(e) {
      widget.toggleVisibility(timerange);
    }, false);
  }

  var moveTimerange = function(range) {
    timerange.start += range;
    timerange.end += range;

    updateInputValue();
    fireSubmitEvent();
  }

  var updateInputValue = function() {
    var input = this_.querySelector("input");
    input.value = dateUtil.formatDateTime(timerange.start, timerange.timezone) +
        " - " +
        dateUtil.formatDateTime(timerange.end, timerange.timezone);


    if (timerange.end + config.range >= Date.now()) {
      this_.querySelector(".arrow_right").classList.add("disabled");
    } else {
      this_.querySelector(".arrow_right").classList.remove("disabled");
    }
  }

  var fireSubmitEvent = function() {
    var ev = new CustomEvent('submit');
    this_.dispatchEvent(ev);
  }
}


var proto = Object.create(HTMLElement.prototype);
TimeRangePickerComponent.apply(proto);
document.registerElement("f-timerange-picker", { prototype: proto });

