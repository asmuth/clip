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
  var default_range = 5 * dateUtil.kMillisPerMinute; //5 minutes
  var timerange = {};
  timerange.end = Date.now();
  timerange.start = timerange.end - default_range;

  var this_;

  this.createdCallback = function() {
    var tpl = templateUtil.getTemplate("f-timerange-picker-tpl");
    this.appendChild(tpl);
    this_ = this;

    //FIXME better naming
    watchTimerangeMover();


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

  var moveTimerange = function(range) {
    timerange.start += range;
    timerange.end += range;

    updateInputValue();
  }

  var updateInputValue = function() {
    var input = this_.querySelector("input");
    input.value = formatDateTime(timerange.start) +
        " - " +
        formatDateTime(timerange.end);
  }

  var formatDateTime = function(date) {
    var d = new Date(date);
    return [
        d.getFullYear(), "-",
        d.getMonth() + 1, "-",
        d.getDate(), " ",
        d.getHours(), ":",
        d.getMinutes()].join("");
  }
}

var proto = Object.create(HTMLElement.prototype);
TimeRangePickerComponent.apply(proto);
document.registerElement("f-timerange-picker", { prototype: proto });

