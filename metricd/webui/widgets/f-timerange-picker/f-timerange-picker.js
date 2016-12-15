/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

var TimeRangePickerUtil = this.TimeRangePickerUtil || {};
TimeRangePickerUtil.formatDateTime = function(date) {
  function appendLeadingZero(num) {
    if (num < 10) {
      return "0" + num;
    }

    return "" + num;
  }

  var d = new Date(date);
  return [
      d.getFullYear(), "-",
      appendLeadingZero(d.getMonth() + 1), "-",
      appendLeadingZero(d.getDate()), " ",
      appendLeadingZero(d.getHours()), ":",
      appendLeadingZero(d.getMinutes())].join("");
}

/**
  * The time range picker popup handler
  * @param timerange is the shared timerange object
  * @param popup is the popup html elem
  */
var TimeRangePickerPopup = function(timerange, popup) {
  'use strict';

  var submit_callbacks = [];

  this.setSubmitCallback = function(c) {
    submit_callbacks.push(c);
  }

  this.toggleVisibility = function() {
    if (popup.isVisible()) {
      popup.close();
      return;
    }

    render();
    popup.show();
  }

/********************************* private **********************************/

  var render = function() {
    var tpl = templateUtil.getTemplate("f-timerange-picker-popup-tpl");
    var inner = popup.querySelector("f-popup-window .inner");
    DomUtil.clearChildren(inner);
    inner.appendChild(tpl.cloneNode(true));

    watchTimerangeButtons(popup);

    var elem;
    var range;

    /** check if current timerange is predefined **/
    var now = Date.now();
    if (now - timerange.end < 30 * dateUtil.kMillisPerSecond) {
      range = timerange.end - timerange.start;
      elem = popup.querySelector(
          "ul.timeranges button[data-value='" + range + "']");
    }

    /** custom timerange **/
    if (!elem) {
      range = 'custom';
      enableCustomInput();
      elem = popup.querySelector("ul.timeranges button[data-value='custom']");
    }

    updateInput(range);
    elem.classList.add("active");
  }

  var watchTimerangeButtons = function(popup) {
    var buttons = popup.querySelectorAll("ul.timeranges li button");
    for (var i = 0; i < buttons.length; i++) {

      buttons[i].addEventListener("click", function(e) {
        e.preventDefault();

        popup.querySelector("ul.timeranges li button.active").
            classList.remove("active");
        this.classList.add("active");

        var selected_timerange = this.getAttribute('data-value');
        updateInput(selected_timerange);

      }, false);

    }
  }

  var updateInput = function(selected_timerange) {
    if (selected_timerange == 'custom') {
      enableCustomInput();
    } else {
      disableCustomInput();
    }

    updateInputValues(selected_timerange);
  }

  var enableCustomInput = function() {
    var elem = popup.querySelector(".col.custom");
    elem.classList.add("active");

    var start_input = elem.querySelector("input[name='start']");
    start_input.removeAttribute('readonly');
    start_input.focus();

    elem.querySelector("input[name='end']").removeAttribute('readonly');
  }

  var disableCustomInput = function() {
    var elem = popup.querySelector(".col.custom");
    elem.classList.remove("active");

    elem.querySelector("input[name='start']").
        setAttribute('readonly', true);

    elem.querySelector("input[name='end']").
        setAttribute('readonly', true);
  }

  var updateInputValues = function(selected_timerange) {
    var start;
    var end;
    if (selected_timerange == 'custom') {
      start = timerange.start;
      end = timerange.end;

    } else {
      end = Date.now();
      start = end - parseInt(selected_timerange);
    }

    popup.querySelector(".custom input[name='start']").
        value = TimeRangePickerUtil.formatDateTime(start);

    popup.querySelector(".custom input[name='end']").
        value = TimeRangePickerUtil.formatDateTime(end);
  }

  var submit = function() {
    var range = popup.querySelector("ul.timeranges li button.active").
        getAttribute('data-value');

    var start;
    var end;
    if (range == 'custom') {
      start = new Date(popup.querySelector("input[name='start']").value).getTime();
      end = new Date(popup.querySelector("input[name='end']").value).getTime();

    } else {
      end = Date.now();
      start = end - parseInt(range);
    }

    submit_callbacks.forEach(function(c) {
      c({
        start: start,
        end: end
      });
    });

    popup.close();
  }

  /** initialize **/
  popup.querySelector("button.cancel").addEventListener("click", function() {
    popup.close();
  }, false);
  popup.querySelector("button.apply").addEventListener("click", submit, false);
}

var TimeRangePickerComponent = function() {
  'use strict';

  var default_range = 5 * dateUtil.kMillisPerMinute; //5 minutes
  var timerange = {};
  timerange.end = Date.now();
  timerange.start = timerange.end - default_range;

  var this_;
  var popup;

  this.createdCallback = function() {
    var tpl = templateUtil.getTemplate("f-timerange-picker-tpl");
    this.appendChild(tpl);

    this_ = this;
    popup = new TimeRangePickerPopup(timerange, this.querySelector("f-popup"));
    popup.setSubmitCallback(function(new_timerange) {
      console.log(c);
      timerange.start = new_timerange.start;
      timerange.end = new_timerange.end;
      fireSubmitEvent();
    });

    //FIXME better naming
    watchTimerangeMover();
    watchInputClick();

    updateInputValue();
  }

  /**
    * Set the start and end of the selected timerange
    * @param start timestamp in milliseconds
    * @param end timestamp in milliseconds
  **/
  this.setTimerange = function(start, end) {
    timerange.start = start;
    timerange.end = end;

    updateInputValue();
  }

  /**
    * Returns start and end of the selected timerange
  **/
  this.getTimerange = function() {
    return {
      start: timerange.start,
      end: timerange.end
    }
  }

/******************************** private *************************************/

  var watchTimerangeMover = function() {
    this_.querySelector(".arrow_left").addEventListener("click", function(e) {
      moveTimerange(default_range * -1);
    }, false);

    this_.querySelector(".arrow_right").addEventListener("click", function(e) {
      moveTimerange(default_range);
    }, false);
  }

  var watchInputClick = function() {
    this_.querySelector("input").addEventListener("click", function(e) {
      popup.toggleVisibility(timerange);
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
    input.value = TimeRangePickerUtil.formatDateTime(timerange.start) +
        " - " +
        TimeRangePickerUtil.formatDateTime(timerange.end);
  }

  var fireSubmitEvent = function() {
    var ev = new CustomEvent('submit');
    this_.dispatchEvent(ev);
  }
}


var proto = Object.create(HTMLElement.prototype);
TimeRangePickerComponent.apply(proto);
document.registerElement("f-timerange-picker", { prototype: proto });

