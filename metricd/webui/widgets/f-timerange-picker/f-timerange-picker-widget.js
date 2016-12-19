/**
  * The time range picker widget handler
  * @param timerange is the shared timerange object
  * @param widget is the widget html elem
  */
var TimeRangePickerWidget = function(timerange, widget) {
  'use strict';

  var submit_callbacks = [];

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
    var ctrls = widget.querySelectorAll("ul.timezones li button");
    for (var i = 0; i < ctrls.length; i++ ) {
      if (ctrls[i].getAttribute("data-value") == timerange.timezone) {
        ctrls[i].classList.add("active");
      }

      ctrls[i].addEventListener("click", function(e) {
        switchActiveButton("timezones", this);
        submitTimezone(this.getAttribute("data-value"));
      }, false);
    }
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
    new Formatter(input, {
      pattern: '{{9999}}-{{99}}-{{99}} {{99}}:{{99}}'
    });
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
    var selected_range = {
      start: dateUtil.toUTC(new Date(widget.querySelector("input[name='start']").value).getTime()),
      end: dateUtil.toUTC(new Date(widget.querySelector("input[name='end']").value).getTime()),
      timezone: timerange.timezone
    };

    submit(selected_range);
  }

  var submit = function(selected_range) {
    submit_callbacks.forEach(function(callback) {
      callback(selected_range);
    });

    close();
  }

  /** initialize **/
  widget.querySelector("button.cancel").addEventListener("click", function() {
    close();
  }, false);

  /** close widget on ESC keypress **/
  document.addEventListener("keydown", function(e) {
    if (e.keyCode == 27) {
      close();
    }
  }, false);
}

