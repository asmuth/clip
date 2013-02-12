
FnordMetric HTML5 API
---------------------

### Introduction

The FnordMetric UI HTML5 API allows you to plugin real-time data and
charts into any website without having to write code. This is achieved
by including a JavaScript library and using data-* attributes on html
elements to declare the widgets.

The javascript library `fnordmetric-ui.js` is bundled with FnordMetric
Classic and FnordMetric Enterprise, but you can download a copy here (FIXPAUL)
if you want to include it in your project.

FnordMetric UI requires jQuery 1.6.2+

    <script src='fnordmetric-ui.js' type='text/javascript'></script>
    <link href='fnordmetric-ui.css' type='text/css' rel='stylesheet' />


### Reference: Counters

Counters are span or div elements. They are updated to contain the value of
the gauge at one point in time as plain text. Example:

    <span
      data-fnordetric="counter"
      data-gauge="my_gauge"
      data-at="now"
      >0</span>

If the value of `my_gauge` this element is updated to:

    <span data-fnordmetric... >23</span>

These are the valid html attributes for counters:

<table>
  <tr>
    <th>data-fnordmetric <i>(mandatory)</i></th>
    <td>
      must be "counter"
    </td>
  </tr>
  <tr>
    <th>data-gauge <i>(mandatory)</i></th>
    <td>
      name of the gauge to be displayed
    </td>
  </tr>
  <tr>
    <th>data-at</th>
    <td>
      display this value at a certain time, may be a unix timestamp,
      a time definition like `-3hours` (3 hours ago), `-45m` (45m ago)
      or `-3600` (3600 seconds = 1 hour ago).
      <p>You can also get a sum of the last x measurements by setting this to
      e.g. "sum(-3hours)" which will give you the sum of the last three hours.</p>
      <p>You can also get a average of the last x measurements by setting this to
      e.g. "avg(-3hours)" which will give you the average of the last three hours.</p>
      <p>The Default is "now".</p>
    </td>
  </tr>
  <tr>
    <th>data-scale-by</th>
    <td>
      multiply the value with a constant before displaying it (e.g. if you are reporting data
      as cents, but want to display them as dollar/euro, use `data-scale-by="0.01"`
    </td>
  </tr>
  <tr>
    <th>data-autoupdate</th>
    <td>
      refresh this value every N seconds
    </td>
  </tr>
  <tr>
    <th>data-unit</th>
    <td>
      display the value with a unit (e.g. € or ms)
    </td>
  </tr>
</table>
<br />


### Reference: Timeseries

Timeseries widgets are div elements. They act as container elements for timeseries
charts. The chart will auto-size itself to the width of the container div, but the
height has to be configured manually (default is 240px):

    <div
      data-fnordetric="timeseries"
      data-gauges="my_gauge"
      data-since="-2hours"
      data-until="now"
      ></div>

These are the valid html attributes:

<table>
  <tr>
    <th>data-fnordmetric <i>(mandatory)</i></th>
    <td>
      must be "timeseries"
    </td>
  </tr>
  <tr>
    <th>data-gauges <i>(mandatory)</i></th>
    <td>
      comma seperated list of gauges to be dispalyed (mandatory)
    </td>
  </tr>
  <tr>
    <th>data-since,<br />data-until <i>(mandatory)</i></th>
    <td>
      these attributes specify the time range to be displayed. they may be a
      unix timestamp, a time definition like "-3hours" (3 hours ago), "-45m"
      (45m ago) or "-3600" (3600 seconds = 1 hour ago) or "now". To e.g. display
      the last two hours of data use: data-since="-2hours" data-until="now"
    </td>
  </tr>
  <tr>
    <th>data-colors</th>
    <td>
      comma seperated list of series / gauge colors (optional)
    </td>
  </tr>
  <tr>
    <th>data-height</th>
    <td>
    height of the chart (default is 240px)
    </td>
  </tr>
  <tr>
    <th>data-chart-style</th>
    <td>
    render style (either line, area or flow), default is "line"
    </td>
  </tr>
  <tr>
    <th>data-legend</th>
    <td>
    display a legend? (values are "on" or "off"), default is "on"
    </td>
  </tr>
  <tr>
    <th>data-cardinal</th>
    <td>
    use cardinal splines (values are "on" or "off"), default is "off"
    </td>
  </tr>
  <tr>
    <th>data-autoupdate</th>
    <td>
      refresh this chart every N seconds, default is no autorefresh
    </td>
  </tr>
</table>
<br />


### Refreshing / Resizing

FnordMetric UI offers two JavaScript methods: `refresh` and `resize`.

**FnordMetric#refresh**

You are allowed manipulate the data-* attributes at runtime. You could use this to build
e.g. a custom date picker. After changing the data-* attributes, you should tell FnordMetric
to refresh this widget.

Example: Refresh element "my_graph"

    FnordMetric.refresh($("#my_graph"));


**FnordMetric#resize**

Resizes all widgets but does not redraw (you can use this to build responsive layouts)

Example: Resize all widgets

    FnordMetric.resize();

