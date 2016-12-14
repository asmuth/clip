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

  this.toggleVisibility = function() {
    if (popup.isVisible()) {
      popup.close();
      return;
    }

    render();
    popup.show();
  }

  var render = function() {
    var tpl = templateUtil.getTemplate("f-timerange-picker-popup-tpl");
    var inner = popup.querySelector("f-popup-window");
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
      elem = popup.querySelector("ul.timerange button[data-value='custom']");
    }

    updateInputValues(range);
    elem.classList.add("active");
  }

  var watchTimerangeButtons = function(popup) {
    var buttons = popup.querySelectorAll("ul.timeranges li button");
    for (var i = 0; i < buttons.length; i++) {

      buttons[i].addEventListener("click", function(e) {
        popup.querySelector("ul.timeranges li button.active").
            classList.remove("active");
        this.classList.add("active");

        var selected_timerange = this.getAttribute('data-value');
        updateInputValues(selected_timerange);

        if (selected_timerange == 'custom') {
          enableCustomInput();
        }
      }, false);

    }
  }

  var enableCustomInput = function() {
    var elem = popup.querySelector(".col.custom");
    // TODO set active class

    var start_input = elem.querySelector("input[name='start']");
    start_input.focus();
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

    popup.querySelector("input[name='start']").
        value = TimeRangePickerUtil.formatDateTime(start);

    popup.querySelector("input[name='end']").
        value = TimeRangePickerUtil.formatDateTime(end);
  }
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
  }

  /**
    * Returns start and end of the selected timerange
  **/
  this.getTimerange = function() {
    return {
      start: timerange.start,
      end: timerang.end
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
  }

  var updateInputValue = function() {
    var input = this_.querySelector("input");
    input.value = TimeRangePickerUtil.formatDateTime(timerange.start) +
        " - " +
        TimeRangePickerUtil.formatDateTime(timerange.end);
  }

}


var proto = Object.create(HTMLElement.prototype);
TimeRangePickerComponent.apply(proto);
document.registerElement("f-timerange-picker", { prototype: proto });

