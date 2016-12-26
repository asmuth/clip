/**
  * The time range picker widget handler
  * @param timerange is the shared timerange object
  * @param widget is the widget html elem
  */
var TimeRangePickerWidget = function(timerange, widget) {
  'use strict';

  var submit_callbacks = [];
  var calendar;

  this.setSubmitCallback = function(c) {
    submit_callbacks.push(c);
  }

  this.toggleVisibility = function() {
    if (widget.classList.contains('active')) {
      close();
    } else {
      show();
    }
  }

  this.close = function() {
    close();
  }

/********************************* private **********************************/

  var initialize = function() {
    calendar = new TimeRangePickerCalendar();
    calendar.setSubmitCallback(function(date) {
      //FIXME handle start and end
      var selected_range = {
        start: date.getTime(),
        end: timerange.end,
        timezone: timerange.timezone
      };

      submit(selected_range);
    });

    widget.querySelector("button.cancel").addEventListener("click", function() {
      close();
    }, false);

    widget.querySelector("button.apply").addEventListener("click", function() {
      submitCustom();
    }, false);

    /** close widget on ESC keypress **/
    document.addEventListener("keydown", function(e) {
      if (e.keyCode == 27) {
        close();
      }
    }, false);

    document.addEventListener("click", function(e) {
      close();
    }, false);
  }

  var show = function() {
    render();
    widget.classList.add('active');
  }

  var close = function() {
    widget.classList.remove('active');
  }

  var render = function() {
    var tpl = templateUtil.getTemplate("f-timerange-picker-widget-tpl");
    var inner = widget.querySelector(".inner");
    DomUtil.clearChildren(inner);
    inner.appendChild(tpl.cloneNode(true));

    watchTimezoneControls();
    watchTimerangeButtons();
    watchCustomSubmit();
    watchCalendarClick();

    enforceInputFormat(widget.querySelector("input[name='start']"));
    enforceInputFormat(widget.querySelector("input[name='end']"));

    var elem;
    var range;

    /** check if current timerange is predefined **/
    var now = Date.now();
    if (now - timerange.end < 30 * dateUtil.kMillisPerSecond) {
      range = timerange.end - timerange.start;
      elem = widget.querySelector(
          "ul.timeranges button[data-value='" + range + "']");
    }

    /** custom timerange **/
    if (!elem) {
      range = 'custom';
      enableCustomInput();
      elem = widget.querySelector("ul.timeranges button[data-value='custom']");
    }

    updateInput(range);
    elem.classList.add("active");
  }

  var watchTimezoneControls = function() {
    var dropdown = widget.querySelector("f-dropdown.timezones");

    if (timerange.timezone) {
      dropdown.setValue(timerange.timezone);
    }

    dropdown.addEventListener("select", function(e) {
      submitTimezone(this.getValue());
    }, false);
  }

  var watchTimerangeButtons = function() {
    /** watch buttons with predefined timeranges */
    var buttons = widget.querySelectorAll("ul.timeranges li button.defined");
    for (var i = 0; i < buttons.length; i++) {

      buttons[i].addEventListener("click", function(e) {
        switchActiveButton("timeranges", this);
        submitPredefined(this);
      }, false);

    }

    /** watch custom time range buttons **/
    var custom_buttons = {
      btn: widget.querySelector("button.custom"),
      link: widget.querySelector("a.custom")
    }

    for (var k in custom_buttons) {

      custom_buttons[k].addEventListener("click", function(e) {
        switchActiveButton("timeranges", custom_buttons.btn);
        enableCustomInput();
      }, false);

    }
  }

  var watchCustomSubmit = function() {
    widget.querySelector(".custom form").addEventListener("submit", function(e) {
      e.preventDefault();
      submitCustom();
    }, false);
  }

  var watchCalendarClick = function() {
    var calendar_box_start = widget.querySelector(".calendar_box.start");
    var calendar_box_end = widget.querySelector(".calendar_box.end");

    /** close calendar on click outside of the calendar elem **/
    widget.addEventListener("click", function(e) {
      DomUtil.clearChildren(calendar_box_end);
      DomUtil.clearChildren(calendar_box_start);
    }, false);

    /** don't close calendar on click within calendar elem **/
    calendar_box_start.addEventListener("click", function(e) {
      e.stopPropagation();
    }, false);

    calendar_box_end.addEventListener("click", function(e) {
      e.stopPropagation();
    }, false);

    /** render start date calendar **/
    widget.querySelector(".icon.calendar.start").addEventListener("click", function(e) {
      e.stopPropagation();

      DomUtil.clearChildren(calendar_box_end);
      DomUtil.clearChildren(calendar_box_start);
      calendar.render(calendar_box_start, {
        min: null,
        max: new Date(timerange.end),
        selected: new Date(timerange.start)
      });
    }, false);

    /** render end date calendar **/
    widget.querySelector(".icon.calendar.end").addEventListener("click", function(e) {
      e.stopPropagation();

      DomUtil.clearChildren(calendar_box_start);
      DomUtil.clearChildren(calendar_box_end);
      calendar.render(calendar_box_end, {
        min: new Date(timerange.start),
        max: new Date(),
        selected: new Date(timerange.end)
      });
    }, false);
  }

  var switchActiveButton = function(list_class, btn) {
    widget.querySelector("ul." + list_class + " li button.active").
        classList.remove("active");

    btn.classList.add("active");
  }

  var updateInput = function(selected_timerange) {
    var start;
    var end;
    if (selected_timerange == 'custom') {
      start = timerange.start;
      end = timerange.end;

      enableCustomInput();

    } else {
      end = Date.now();
      start = end - parseInt(selected_timerange);
    }

    widget.querySelector(".custom input[name='start']").
        value = dateUtil.formatDateTime(start, timerange.timezone);

    widget.querySelector(".custom input[name='end']").
        value = dateUtil.formatDateTime(end, timerange.timezone);
  }

  var enableCustomInput = function() {
    var elem = widget.querySelector(".col.custom");
    elem.classList.add("active");

    var start_input = elem.querySelector("input[name='start']");
    start_input.removeAttribute('readonly');
    start_input.focus();

    elem.querySelector("input[name='end']").removeAttribute('readonly');

  }

  var enforceInputFormat = function(input) {
    /** enforce YYYY-mm-dd HH:MM format **/
    new Formatter(input, {
      pattern: '{{9999}}-{{99}}-{{99}} {{99}}:{{99}}'
    });

    input.addEventListener("focus", function(e) {
      this.classList.remove("invalid");
    }, false);

    input.addEventListener("blur", function(e) {
      validateInput(this);
    }, false);
  }

  var validateInput = function(input) {
    /** as the format is already checked, we only have to validate the length **/
    if (input.value.length != 16) {
      input.classList.add("invalid");
      return false;
    }

    return true;
  }

  var disableCustomInput = function() {
    var elem = widget.querySelector(".col.custom");
    elem.classList.remove("active");

    elem.querySelector("input[name='start']").
        setAttribute('readonly', true);

    elem.querySelector("input[name='end']").
        setAttribute('readonly', true);
  }

  var submitTimezone = function(selected_timezone) {
    var selected_range = {
      start: timerange.start,
      end: timerange.end,
      timezone: selected_timezone
    };

    submit(selected_range);
  }

  var submitPredefined = function(elem) {
    var selected_range = {};
    selected_range.end = Date.now();
    selected_range.start =
        selected_range.end - parseInt(elem.getAttribute('data-value'));
    selected_range.timezone = timerange.timezone;

    submit(selected_range);
  }

  var submitCustom = function() {
    var start_input = widget.querySelector("input[name='start']");
    var end_input = widget.querySelector("input[name='end']");
    if (!validateInput(start_input) || !validateInput(end_input)) {
      //TODO render error message
      return;
    }

    var start = new Date(start_input.value).getTime();
    var end = new Date(end_input.value).getTime();
    if (start > end) {
      //TODO render error message 
      return;
    }

    if (timerange.timezone != "local") {
      start = dateUtil.toUTC(start);
      end = dateUtil.toUTC(end);
    }

    submit({
      start: start,
      end: end,
      timezone: timerange.timezone
    });
  }

  var submit = function(selected_range) {
    submit_callbacks.forEach(function(callback) {
      callback(selected_range);
    });

    close();
  }

  initialize();
}

